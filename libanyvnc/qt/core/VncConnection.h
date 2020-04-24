/*
 * VncConnection.h - declaration of VncConnection class
 *
 * Copyright (c) 2008-2020 Tobias Junghans <tobydox@veyon.io>
 *
 * This file is part of AnyVNC - https://anyvnc.com
 *
 * code partly taken from KRDC / vncclientthread.h:
 * Copyright (C) 2007-2008 Urs Wolfer <uwolfer @ kde.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program (see COPYING); if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 */

#pragma once

#include <QElapsedTimer>
#include <QImage>
#include <QMutex>
#include <QQueue>
#include <QReadWriteLock>
#include <QThread>
#include <QWaitCondition>

#include "AnyVncQtCore.h"

// TODO: decouple from libvncclient through ClientBackend plugin

using rfbClient = struct _rfbClient;

namespace AnyVncQt
{

namespace Core
{

class VncEvent;

class ANYVNC_QT_CORE_EXPORT VncConnection : public QThread
{
	Q_OBJECT
public:
	using SocketType = int;

	enum class Quality
	{
		Thumbnail,
		Screenshot,
		RemoteControl,
		Default
	} ;

	enum class FramebufferState
	{
		Invalid,
		Initialized,
		Valid
	} ;

	enum class State
	{
		None,
		Disconnected,
		Connecting,
		HostOffline,
		ServiceUnreachable,
		AuthenticationFailed,
		ConnectionFailed,
		Connected
	} ;
	Q_ENUM(State)

	explicit VncConnection( QObject *parent = nullptr );
	~VncConnection() override;

	static void initLogging( bool debug );

	QImage image();

	void restart();
	void stop();
	void stopAndDeleteLater();

	void setHost( const QString& host );
	void setPort( int port );

	State state() const
	{
		return m_state;
	}

	bool isConnected() const
	{
		return state() == State::Connected && isRunning();
	}

	const QString& host() const
	{
		return m_host;
	}

	auto port() const
	{
		return m_port;
	}

	const QString& password() const
	{
		return m_password;
	}

	void setPassword( const QString& password )
	{
		m_password = password;
	}

	void setQuality( Quality quality )
	{
		m_quality = quality ;
	}

	void setServerReachable();

	void enqueueEvent( VncEvent* event, bool wake );
	bool isEventQueueEmpty();

	/** \brief Returns whether framebuffer data is valid, i.e. at least one full FB update received */
	bool hasValidFrameBuffer() const
	{
		return m_framebufferState == FramebufferState::Valid;
	}

	void setScaledSize( QSize s );

	QImage scaledScreen();

	void setFramebufferUpdateInterval( int interval );

	void rescaleScreen();

	static constexpr int VncConnectionTag = 0x590123;

	static void* clientData( rfbClient* client, int tag );
	void setClientData( int tag, void* data );

	void mouseEvent( int x, int y, uint buttonMask );
	void keyEvent( unsigned int key, bool pressed );
	void clientCut( const QString& text );

	SocketType socket() const;

Q_SIGNALS:
	void connectionPrepared();
	void connectionEstablished();
	void imageUpdated( int x, int y, int w, int h );
	void framebufferUpdateComplete();
	void framebufferSizeChanged( int w, int h );
	void cursorPosChanged( int x, int y );
	void cursorShapeUpdated( const QPixmap& cursorShape, int xh, int yh );
	void gotCut( const QString& text );
	void stateChanged();

protected:
	void run() override;

private:
	// intervals and timeouts
	static constexpr int ThreadTerminationTimeout = 30000;
	static constexpr int ConnectTimeout = 5000;
	static constexpr int ConnectionRetryInterval = 1000;
	static constexpr int MessageWaitTimeout = 500;
	static constexpr int FastFramebufferUpdateInterval = 100;
	static constexpr int FramebufferUpdateWatchdogTimeout = 10000;
	static constexpr int SocketKeepaliveIdleTime = 1000;
	static constexpr int SocketKeepaliveInterval = 500;
	static constexpr int SocketKeepaliveCount = 5;

	// RFB parameters
	using RfbPixel = uint32_t;
	static constexpr int RfbBitsPerSample = 8;
	static constexpr int RfbSamplesPerPixel = 3;
	static constexpr int RfbBytesPerPixel = sizeof(RfbPixel);
	static constexpr int VncDefaultPort = 5900;

	enum class ControlFlag {
		ScaledScreenNeedsUpdate = 0x01,
		ServerReachable = 0x02,
		TerminateThread = 0x04,
		RestartConnection = 0x08,
	};

	void establishConnection();
	void handleConnection();
	void closeConnection();

	void setState( State state );

	void setControlFlag( ControlFlag flag, bool on );
	bool isControlFlagSet( ControlFlag flag );

	bool initFrameBuffer( rfbClient* client );
	void finishFrameBufferUpdate();

	void sendEvents();

	// hooks for LibVNCClient
	static int8_t hookInitFrameBuffer( rfbClient* client );
	static void hookUpdateFB( rfbClient* client, int x, int y, int w, int h );
	static void hookFinishFrameBufferUpdate( rfbClient* client );
	static int8_t hookHandleCursorPos( rfbClient* client, int x, int y );
	static void hookCursorShape( rfbClient* client, int xh, int yh, int w, int h, int bpp );
	static void hookCutText( rfbClient* client, const char *text, int textlen );
	static char* hookGetPassword( rfbClient* client );
	static void rfbClientLogDebug( const char* format, ... );
	static void rfbClientLogNone( const char* format, ... );
	static void framebufferCleanup( void* framebuffer );

	// states and flags
	std::atomic<State> m_state{State::Disconnected};
	std::atomic<FramebufferState> m_framebufferState{FramebufferState::Invalid};
	QAtomicInteger<uint> m_controlFlags{};

	// connection parameters and data
	rfbClient* m_client{nullptr};
	Quality m_quality{Quality::Default};
	QString m_host{};
	int m_port{VncDefaultPort};
	QString m_password{};

	// thread and timing control
	QMutex m_globalMutex{};
	QMutex m_eventQueueMutex{};
	QWaitCondition m_updateIntervalSleeper{};
	QAtomicInt m_framebufferUpdateInterval{0};
	QElapsedTimer m_framebufferUpdateWatchdog{};

	// queue for RFB and custom events
	QQueue<VncEvent *> m_eventQueue{};

	// framebuffer data and thread synchronization objects
	QImage m_image{};
	QImage m_scaledScreen{};
	QSize m_scaledSize{};
	QReadWriteLock m_imgLock{};

};

}

}
