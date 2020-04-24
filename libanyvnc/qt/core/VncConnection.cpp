/*
 * VncConnection.cpp - implementation of VncConnection class
 *
 * Copyright (c) 2008-2020 Tobias Junghans <tobydox@veyon.io>
 *
 * This file is part of AnyVNC - https://anyvnc.com
 *
 * code partly taken from KRDC / vncclientthread.cpp:
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

#include <rfb/rfbclient.h>

#include <QBitmap>
#include <QDeadlineTimer>
#include <QMutexLocker>
#include <QPixmap>

#include "AnyVncQt.h"
#include "VncConnection.h"
#include "VncEvents.h"


namespace AnyVncQt::Core
{

// TODO: decouple from libvncclient through ClientBackend plugin

rfbBool VncConnection::hookInitFrameBuffer( rfbClient* client )
{
	auto connection = static_cast<VncConnection *>( clientData( client, VncConnectionTag ) );
	if( connection )
	{
		return connection->initFrameBuffer( client );
	}

	return false;
}




void VncConnection::hookUpdateFB( rfbClient* client, int x, int y, int w, int h )
{
	auto connection = static_cast<VncConnection *>( clientData( client, VncConnectionTag ) );
	if( connection )
	{
		Q_EMIT connection->imageUpdated( x, y, w, h );
	}
}




void VncConnection::hookFinishFrameBufferUpdate( rfbClient* client )
{
	auto connection = static_cast<VncConnection *>( clientData( client, VncConnectionTag ) );
	if( connection )
	{
		connection->finishFrameBufferUpdate();
	}
}




rfbBool VncConnection::hookHandleCursorPos( rfbClient* client, int x, int y )
{
	auto connection = static_cast<VncConnection *>( clientData( client, VncConnectionTag ) );
	if( connection )
	{
		Q_EMIT connection->cursorPosChanged( x, y );
	}

	return true;
}




void VncConnection::hookCursorShape( rfbClient* client, int xh, int yh, int w, int h, int bpp )
{
	if( bpp != 4 )
	{
		avqWarning() << QThread::currentThreadId() << "bytes per pixel != 4";
		return;
	}

	QImage alpha( client->rcMask, w, h, QImage::Format_Indexed8 );
	alpha.setColorTable( { qRgb(255,255,255), qRgb(0,0,0) } );

	QPixmap cursorShape( QPixmap::fromImage( QImage( client->rcSource, w, h, QImage::Format_RGB32 ) ) );
	cursorShape.setMask( QBitmap::fromImage( alpha ) );

	auto connection = static_cast<VncConnection *>( clientData( client, VncConnectionTag ) );
	if( connection )
	{
		Q_EMIT connection->cursorShapeUpdated( cursorShape, xh, yh );
	}
}



void VncConnection::hookCutText( rfbClient* client, const char* text, int textlen )
{
	auto connection = static_cast<VncConnection *>( clientData( client, VncConnectionTag ) );
	const auto cutText = QString::fromUtf8( text, textlen );

	if( connection && cutText.isEmpty() == false  )
	{
		Q_EMIT connection->gotCut( cutText );
	}
}



char* VncConnection::hookGetPassword( rfbClient* client )
{
	auto connection = static_cast<VncConnection *>( clientData( client, VncConnectionTag ) );

	return strdup(connection->password().toUtf8().constData());
}



void VncConnection::rfbClientLogDebug( const char* format, ... )
{
	va_list args;
	va_start( args, format );

	static constexpr int MaxMessageLength = 256;
	std::array<char, MaxMessageLength> message{};

	vsnprintf( message.data(), sizeof(message), format, args );
	message[MaxMessageLength-1] = 0;

	va_end(args);

	avqDebug() << QThread::currentThreadId() << message.data();
}




void VncConnection::rfbClientLogNone( const char* format, ... )
{
	Q_UNUSED(format)
}



void VncConnection::framebufferCleanup( void* framebuffer )
{
	delete[] static_cast<RfbPixel *>( framebuffer );
}




VncConnection::VncConnection( QObject* parent ) :
	QThread( parent )
{
}



VncConnection::~VncConnection()
{
	stop();

	if( isRunning() )
	{
		avqWarning() << "Waiting for VNC connection thread to finish.";
		wait( ThreadTerminationTimeout );
	}

	if( isRunning() )
	{
		avqWarning() << "Terminating hanging VNC connection thread!";

		terminate();
		wait();
	}
}



void VncConnection::initLogging( bool debug )
{
	if( debug )
	{
		rfbClientLog = rfbClientLogDebug;
		rfbClientErr = rfbClientLogDebug;
	}
	else
	{
		rfbClientLog = rfbClientLogNone;
		rfbClientErr = rfbClientLogNone;
	}
}



QImage VncConnection::image()
{
	QReadLocker locker( &m_imgLock );
	return m_image;
}



void VncConnection::restart()
{
	setControlFlag( ControlFlag::RestartConnection, true );
}



void VncConnection::stop()
{
	setClientData( VncConnectionTag, nullptr );

	m_scaledScreen = {};

	setControlFlag( ControlFlag::TerminateThread, true );

	m_updateIntervalSleeper.wakeAll();
}



void VncConnection::stopAndDeleteLater()
{
	if( isRunning() )
	{
		connect( this, &VncConnection::finished, this, &VncConnection::deleteLater );
		stop();
	}
	else
	{
		deleteLater();
	}
}



void VncConnection::setHost( const QString& host )
{
	QMutexLocker locker( &m_globalMutex );
	m_host = host;

	// is IPv6-mapped IPv4 address?
	QRegExp rx( QStringLiteral( "::[fF]{4}:(\\d+.\\d+.\\d+.\\d+)" ) );
	if( rx.indexIn( m_host ) == 0 )
	{
		// then use plain IPv4 address as libvncclient cannot handle
		// IPv6-mapped IPv4 addresses on Windows properly
		m_host = rx.cap( 1 );
	}
	else if( m_host == QLatin1String( "::1" ) )
	{
		m_host = QStringLiteral("127.0.0.1");
	}
	else if( m_host.count( QLatin1Char(':') ) == 1 )
	{
		// hostname + port number?
		QRegExp rx2( QStringLiteral("(.*[^:]):(\\d+)$") );
		if( rx2.indexIn( m_host ) == 0 )
		{
			m_host = rx2.cap( 1 );
			m_port = rx2.cap( 2 ).toInt();
		}
	}
}



void VncConnection::setPort( int port )
{
	if( port >= 0 )
	{
		QMutexLocker locker( &m_globalMutex );
		m_port = port;
	}
}



void VncConnection::setServerReachable()
{
	setControlFlag( ControlFlag::ServerReachable, true );
}



void VncConnection::setScaledSize( QSize s )
{
	QMutexLocker globalLock( &m_globalMutex );

	if( m_scaledSize != s )
	{
		m_scaledSize = s;
		setControlFlag( ControlFlag::ScaledScreenNeedsUpdate, true );
	}
}



QImage VncConnection::scaledScreen()
{
	rescaleScreen();
	return m_scaledScreen;
}



void VncConnection::setFramebufferUpdateInterval( int interval )
{
	m_framebufferUpdateInterval = interval;
}



void VncConnection::rescaleScreen()
{
	if( hasValidFrameBuffer() == false || m_scaledSize.isNull() )
	{
		m_scaledScreen = {};
		return;
	}

	if( isControlFlagSet( ControlFlag::ScaledScreenNeedsUpdate ) == false )
	{
		return;
	}

	QReadLocker locker( &m_imgLock );

	if( m_image.size().isValid() == false )
	{
		return;
	}

	m_scaledScreen = m_image.scaled( m_scaledSize, ::Qt::IgnoreAspectRatio, ::Qt::SmoothTransformation );

	setControlFlag( ControlFlag::ScaledScreenNeedsUpdate, false );
}



void* VncConnection::clientData( rfbClient* client, int tag )
{
	if( client )
	{
		return rfbClientGetClientData( client, reinterpret_cast<void *>( tag ) );
	}

	return nullptr;
}



void VncConnection::setClientData( int tag, void* data )
{
	QMutexLocker globalLock( &m_globalMutex );

	if( m_client )
	{
		rfbClientSetClientData( m_client, reinterpret_cast<void *>( tag ), data );
	}
}



void VncConnection::run()
{
	while( isControlFlagSet( ControlFlag::TerminateThread ) == false )
	{
		establishConnection();
		handleConnection();
		closeConnection();
	}
}



void VncConnection::establishConnection()
{
	QMutex sleeperMutex;

	setState( State::Connecting );
	setControlFlag( ControlFlag::RestartConnection, false );

	m_framebufferState = FramebufferState::Invalid;

	while( isControlFlagSet( ControlFlag::TerminateThread ) == false &&
		   state() != State::Connected ) // try to connect as long as the server allows
	{
		m_client = rfbGetClient( RfbBitsPerSample, RfbSamplesPerPixel, RfbBytesPerPixel );
		m_client->MallocFrameBuffer = hookInitFrameBuffer;
		m_client->canHandleNewFBSize = true;
		m_client->GotFrameBufferUpdate = hookUpdateFB;
		m_client->FinishedFrameBufferUpdate = hookFinishFrameBufferUpdate;
		m_client->HandleCursorPos = hookHandleCursorPos;
		m_client->GotCursorShape = hookCursorShape;
		m_client->GotXCutText = hookCutText;
		m_client->GetPassword = hookGetPassword;

		//TODO: m_client->connectTimeout = ConnectTimeout;
		setClientData( VncConnectionTag, this );

		Q_EMIT connectionPrepared();

		m_globalMutex.lock();

		m_client->serverPort = VncDefaultPort;

		free( m_client->serverHost );
		m_client->serverHost = strdup( m_host.toUtf8().constData() );

		m_globalMutex.unlock();

		setControlFlag( ControlFlag::ServerReachable, false );

		if( rfbInitClient( m_client, nullptr, nullptr ) &&
			isControlFlagSet( ControlFlag::TerminateThread ) == false )
		{
			m_framebufferUpdateWatchdog.restart();

			Q_EMIT connectionEstablished();

			setState( State::Connected );
		}
		else
		{
			// rfbInitClient() calls rfbClientCleanup() when failed
			m_client = nullptr;

			// do not sleep when already requested to stop
			if( isControlFlagSet( ControlFlag::TerminateThread ) )
			{
				break;
			}

			// guess reason why connection failed
			if( isControlFlagSet( ControlFlag::ServerReachable ) == false )
			{
				// TODO: if( VeyonCore::platform().networkFunctions().ping( m_host ) == false )
				{
					setState( State::HostOffline );
				}
/*				else
				{
					setState( State::ServiceUnreachable );
				}*/
			}
			else if( m_framebufferState == FramebufferState::Invalid )
			{
				setState( State::AuthenticationFailed );
			}
			else
			{
				// failed for an unknown reason
				setState( State::ConnectionFailed );
			}

			// wait a bit until next connect
			sleeperMutex.lock();
			if( m_framebufferUpdateInterval > 0 )
			{
				m_updateIntervalSleeper.wait( &sleeperMutex, QDeadlineTimer( m_framebufferUpdateInterval ) );
			}
			else
			{
				// default: retry every second
				m_updateIntervalSleeper.wait( &sleeperMutex, QDeadlineTimer( ConnectionRetryInterval ) );
			}
			sleeperMutex.unlock();
		}
	}
}



void VncConnection::handleConnection()
{
	QMutex sleeperMutex;
	QElapsedTimer loopTimer;

	while( state() == State::Connected &&
		   isControlFlagSet( ControlFlag::TerminateThread ) == false &&
		   isControlFlagSet( ControlFlag::RestartConnection ) == false )
	{
		loopTimer.start();

		const int i = WaitForMessage( m_client, MessageWaitTimeout );
		if( isControlFlagSet( ControlFlag::TerminateThread ) || i < 0 )
		{
			break;
		}

		if( i )
		{
			// handle all available messages
			bool handledOkay = true;
			do {
				handledOkay &= HandleRFBServerMessage( m_client );
			} while( handledOkay && WaitForMessage( m_client, 0 ) );

			if( handledOkay == false )
			{
				break;
			}
		}

		sendEvents();

		const auto remainingUpdateInterval = m_framebufferUpdateInterval - loopTimer.elapsed();

		if( m_framebufferState == FramebufferState::Initialized ||
			m_framebufferUpdateWatchdog.elapsed() >= qMax<qint64>( 2*m_framebufferUpdateInterval, FramebufferUpdateWatchdogTimeout ) )
		{
			SendFramebufferUpdateRequest( m_client, 0, 0, m_client->width, m_client->height, false );

			const auto remainingFastUpdateInterval = FastFramebufferUpdateInterval - loopTimer.elapsed();

			sleeperMutex.lock();
			m_updateIntervalSleeper.wait( &sleeperMutex, QDeadlineTimer( remainingFastUpdateInterval ) );
			sleeperMutex.unlock();
		}
		else if( m_framebufferState == FramebufferState::Valid &&
			remainingUpdateInterval > 0 &&
			isControlFlagSet( ControlFlag::TerminateThread ) == false )
		{
			sleeperMutex.lock();
			m_updateIntervalSleeper.wait( &sleeperMutex, QDeadlineTimer( remainingUpdateInterval ) );
			sleeperMutex.unlock();
		}

		sendEvents();
	}
}



void VncConnection::closeConnection()
{
	if( m_client )
	{
		rfbClientCleanup( m_client );
		m_client = nullptr;
	}

	setState( State::Disconnected );
}



void VncConnection::setState( State state )
{
	if( m_state.exchange( state ) != state )
	{
		Q_EMIT stateChanged();
	}
}



void VncConnection::setControlFlag( VncConnection::ControlFlag flag, bool on )
{
	if( on )
	{
		m_controlFlags |= uint( flag );
	}
	else
	{
		m_controlFlags &= ~uint( flag );
	}
}



bool VncConnection::isControlFlagSet( VncConnection::ControlFlag flag )
{
	return m_controlFlags & uint( flag );
}




bool VncConnection::initFrameBuffer( rfbClient* client )
{
	if( client->format.bitsPerPixel != RfbBitsPerSample * RfbBytesPerPixel )
	{
		avqCritical() << "Bits per pixel does not match" << client->format.bitsPerPixel;
		return false;
	}

	const auto pixelCount = uint32_t(client->width) * uint32_t(client->height);

	client->frameBuffer = reinterpret_cast<uint8_t *>( new RfbPixel[pixelCount] );

	memset( client->frameBuffer, '\0', pixelCount*RfbBytesPerPixel );

	// initialize framebuffer image which just wraps the allocated memory and ensures cleanup after last
	// image copy using the framebuffer gets destroyed
	m_imgLock.lockForWrite();
	m_image = QImage( client->frameBuffer, client->width, client->height, QImage::Format_RGB32, framebufferCleanup, client->frameBuffer );
	m_imgLock.unlock();

	// set up pixel format according to QImage
	client->format.redShift = 16;
	client->format.greenShift = 8;
	client->format.blueShift = 0;
	client->format.redMax = 0xff;
	client->format.greenMax = 0xff;
	client->format.blueMax = 0xff;

	client->appData.encodingsString = "zrle ultra copyrect hextile zlib corre rre raw";
	client->appData.useRemoteCursor = false;
	client->appData.compressLevel = 0;
	client->appData.useBGR233 = false;
	client->appData.qualityLevel = 9;
	client->appData.enableJPEG = false;

	switch( m_quality )
	{
	case Quality::Screenshot:
		// make sure to use lossless raw encoding
		client->appData.encodingsString = "raw";
		break;
	case Quality::RemoteControl:
		client->appData.useRemoteCursor = true;
		break;
	case Quality::Thumbnail:
		client->appData.compressLevel = 9;
		client->appData.qualityLevel = 5;
		client->appData.enableJPEG = true;
		break;
	default:
		break;
	}

	m_framebufferState = FramebufferState::Initialized;

	Q_EMIT framebufferSizeChanged( client->width, client->height );

	return true;
}



void VncConnection::finishFrameBufferUpdate()
{
	m_framebufferUpdateWatchdog.restart();

	m_framebufferState = FramebufferState::Valid;
	setControlFlag( ControlFlag::ScaledScreenNeedsUpdate, true );

	Q_EMIT framebufferUpdateComplete();
}



void VncConnection::sendEvents()
{
	m_eventQueueMutex.lock();

	while( m_eventQueue.isEmpty() == false )
	{
		auto event = m_eventQueue.dequeue();

		// unlock the queue mutex during the runtime of ClientEvent::fire()
		m_eventQueueMutex.unlock();

		if( isControlFlagSet( ControlFlag::TerminateThread ) == false )
		{
			event->fire( m_client );
		}

		delete event;

		// and lock it again
		m_eventQueueMutex.lock();
	}

	m_eventQueueMutex.unlock();
}



void VncConnection::enqueueEvent( VncEvent* event, bool wake )
{
	if( state() != State::Connected )
	{
		return;
	}

	m_eventQueueMutex.lock();
	m_eventQueue.enqueue( event );
	m_eventQueueMutex.unlock();

	if( wake )
	{
		m_updateIntervalSleeper.wakeAll();
	}
}



bool VncConnection::isEventQueueEmpty()
{
	QMutexLocker lock( &m_eventQueueMutex );
	return m_eventQueue.isEmpty();
}



void VncConnection::mouseEvent( int x, int y, uint buttonMask )
{
	enqueueEvent( new VncPointerEvent( x, y, buttonMask ), true );
}



void VncConnection::keyEvent( unsigned int key, bool pressed )
{
	enqueueEvent( new VncKeyEvent( key, pressed ), true );
}



void VncConnection::clientCut( const QString& text )
{
	enqueueEvent( new VncClientCutEvent( text ), true );
}



VncConnection::SocketType VncConnection::socket() const
{
	return m_client->sock;
}

}
