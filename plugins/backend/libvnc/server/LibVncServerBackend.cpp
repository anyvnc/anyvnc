/*
 * LibVncServerBackend.cpp
 *
 * Copyright (c) 2020 Tobias Junghans <tobydox@veyon.io>
 *
 * This file is part of AnyVNC - https://anyvnc.com
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

#include <iostream>

#include "LibVncServerBackend.h"

#include "libanyvnc/core/Server.h"
#include "libanyvnc/interfaces/Framebuffer.h"

extern "C" {
#include <rfb/rfbregion.h>
}


namespace AnyVnc
{

static void handleClientGone( rfbClientPtr cl )
{
	std::cout << cl->host;
}


static rfbNewClientAction handleNewClient( rfbClientPtr cl )
{
	cl->clientGoneHook = handleClientGone;

	std::cout << "New client connection from host" << cl->host;

	return RFB_CLIENT_ACCEPT;
}


static void handleClipboardText( char* str, int len, rfbClientPtr cl )
{
	str[len] = '\0';

	const auto server = reinterpret_cast<Core::Server *>( cl->screen->screenData );
	if( server )
	{
		server->clipboard()->setText( str );
	}
}


static void handleKeyEvent( rfbBool down, rfbKeySym keySym, rfbClientPtr cl )
{
	const auto server = reinterpret_cast<Core::Server *>( cl->screen->screenData );
	if( server )
	{
		server->keyboard()->synthesizeKeyEvent( keySym, down );
	}
}



static void handlePointerEvent( int buttons, int x, int y, rfbClientPtr cl )
{
	using Button = Interfaces::PointingDevice::Button;

	const auto server = reinterpret_cast<Core::Server *>( cl->screen->screenData );
	if( server )
	{
		if( cl->lastPtrX != x || cl->lastPtrY != y )
		{
			server->pointingDevice()->move( { x, y } );
		}

		const auto handleButton = [buttons, cl, server]( auto buttonMask, Button button )
		{
			if( ( buttons & buttonMask ) && !( cl->lastPtrButtons & buttonMask ) )
			{
				server->pointingDevice()->pressButton( button );
			}
			else if( !( buttons & buttonMask ) && cl->lastPtrButtons & buttonMask )
			{
				server->pointingDevice()->releaseButton( button );
			}
		};

		handleButton( rfbButton1Mask, Button::Left );
		handleButton( rfbButton2Mask, Button::Middle );
		handleButton( rfbButton3Mask, Button::Right );

		if( buttons & rfbWheelUpMask )
		{
			server->pointingDevice()->scrollUp();
		}

		if( buttons & rfbWheelDownMask )
		{
			server->pointingDevice()->scrollDown();
		}
	}
}



LibVncServerBackend::~LibVncServerBackend()
{
	shutdown();
}



bool LibVncServerBackend::initialize( Core::Server* server )
{
	m_server = server;

	m_rfbScreen = rfbGetScreen( nullptr, nullptr,
								   m_server->framebuffer()->size().width(),
								   m_server->framebuffer()->size().height(),
								   8, 3, 4 ); // TODO: honor FB/screen depth

	if( m_rfbScreen == nullptr )
	{
		return false;
	}

	m_password = server->password();

	if( m_password.size() > 0 )
	{
		m_passwords[0] = m_password.c_str();
	}

	m_rfbScreen->desktopName = "AnyVNC";
	m_rfbScreen->frameBuffer = reinterpret_cast<char *>( m_server->framebuffer()->data() );
	m_rfbScreen->port = m_server->port();
	m_rfbScreen->kbdAddEvent = handleKeyEvent;
	m_rfbScreen->ptrAddEvent = handlePointerEvent;
	m_rfbScreen->newClientHook = handleNewClient;
	m_rfbScreen->setXCutText = handleClipboardText;

	m_rfbScreen->authPasswdData = m_passwords.data();
	m_rfbScreen->passwordCheck = rfbCheckPasswordByList;

	m_rfbScreen->serverFormat.redShift = 16;
	m_rfbScreen->serverFormat.greenShift = 8;
	m_rfbScreen->serverFormat.blueShift = 0;

	m_rfbScreen->serverFormat.redMax = 255;
	m_rfbScreen->serverFormat.greenMax = 255;
	m_rfbScreen->serverFormat.blueMax = 255;

	m_rfbScreen->serverFormat.trueColour = true;

	m_rfbScreen->alwaysShared = true;
	m_rfbScreen->handleEventsEagerly = true;
	m_rfbScreen->deferUpdateTime = 5;

	m_rfbScreen->screenData = server;

	rfbInitServer( m_rfbScreen );

	rfbMarkRectAsModified( m_rfbScreen, 0, 0, m_rfbScreen->width, m_rfbScreen->height );

	return true;
}



bool LibVncServerBackend::handleFramebufferUpdate()
{
	bool modified = false;

	const auto updateFlags = m_server->framebuffer()->update( [this, &modified]( Types::Rectangle rect ) {
		rfbMarkRectAsModified( m_rfbScreen, rect.left(), rect.top(), rect.right(), rect.bottom() );
		modified = true;
	} );

	if( updateFlags & Interfaces::Framebuffer::UpdateFlag::SizeChanged )
	{
		rfbClientPtr cl;
		auto iterator = rfbGetClientIterator( m_rfbScreen );
		while( ( cl = rfbClientIteratorNext(iterator) ) != nullptr )
		{
			cl->newFBSizePending = 1;
		}

		modified = true;
	}

	return modified;
}



bool LibVncServerBackend::hasConnectedClients() const
{
	return m_rfbScreen->clientHead != nullptr;
}


bool LibVncServerBackend::hasPendingClientUpdateRequests() const
{
	for( auto clientPtr = m_rfbScreen->clientHead; clientPtr != nullptr; clientPtr = clientPtr->next )
	{
		if( sraRgnEmpty( clientPtr->requestedRegion ) == false )
		{
			return true;
		}
	}

	return false;
}



bool LibVncServerBackend::processEvents( int timeout )
{
	return rfbProcessEvents( m_rfbScreen, long(timeout) * MicroSecondsPerMilliSecond );
}



bool LibVncServerBackend::shutdown()
{
	if( m_rfbScreen )
	{
		rfbShutdownServer( m_rfbScreen, true );
		rfbScreenCleanup( m_rfbScreen );

		m_rfbScreen = nullptr;
	}

	return true;
}

}

ANYVNC_EXPORT_PLUGIN(AnyVnc::LibVncServerBackend)
