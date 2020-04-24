/*
 * Server.cpp
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

#include "Server.h"
#include "PluginLoader.h"

namespace AnyVnc
{

namespace Core
{

bool Server::run()
{
	if( createFramebuffer() == false ||
		createKeyboard() == false ||
		createPointingDevice() == false ||
		createClipboard() == false ||
		createBackend() == false )
	{
		shutdown();
		return false;
	}

	m_quit = false;

	// TODO: multi-monitor support
	while( m_quit == false &&
		   m_framebuffer->size() == m_framebuffer->availableScreens().at(0).geometry() )
	{
		auto timeout = IdleTimeout;
		if( m_backend->hasConnectedClients() )
		{
			timeout = NonIdleTimeout;
			m_backend->handleFramebufferUpdate();
		}

		m_backend->processEvents( timeout );
	}

	shutdown();

	return true;
}



void Server::quit()
{
	m_quit = true;
}



bool Server::createFramebuffer()
{
	m_framebuffer = PluginLoader().createAndInitialize<Framebuffer>( this );

	return m_framebuffer != nullptr;
}



bool Server::createPointingDevice()
{
	m_pointingDevice = PluginLoader().createAndInitialize<PointingDevice>( this );

	return m_pointingDevice != nullptr;
}



bool Server::createClipboard()
{
	m_clipboard = PluginLoader().createAndInitialize<Clipboard>( this );

	return m_clipboard != nullptr;
}



bool Server::createKeyboard()
{
	m_keyboard = PluginLoader().createAndInitialize<Keyboard>( this );

	return m_keyboard != nullptr;
}



bool Server::createBackend()
{
	m_backend = PluginLoader().createAndInitialize<Backend>( this );

	return m_backend != nullptr;
}



void Server::shutdown()
{
	if( m_backend )
	{
		m_backend->shutdown();
	}

	delete m_backend;
	m_backend = nullptr;

	delete m_clipboard;
	m_clipboard = nullptr;

	delete m_pointingDevice;
	m_pointingDevice = nullptr;

	delete m_keyboard;
	m_keyboard = nullptr;

	delete m_framebuffer;
	m_framebuffer = nullptr;

}

}

}
