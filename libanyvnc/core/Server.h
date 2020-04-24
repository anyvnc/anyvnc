/*
 * Server.h
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

#include <atomic>

#include "libanyvnc/core/AnyVncCore.h"
#include "libanyvnc/interfaces/Clipboard.h"
#include "libanyvnc/interfaces/Framebuffer.h"
#include "libanyvnc/interfaces/Keyboard.h"
#include "libanyvnc/interfaces/PointingDevice.h"
#include "libanyvnc/interfaces/ServerBackend.h"

namespace AnyVnc
{

namespace Core
{

class ANYVNC_CORE_EXPORT Server
{
public:
	using Clipboard = Interfaces::Clipboard;
	using Framebuffer = Interfaces::Framebuffer;
	using Keyboard = Interfaces::Keyboard;
	using PointingDevice = Interfaces::PointingDevice;
	using Backend = Interfaces::ServerBackend;

	Server() = default;
	virtual ~Server() = default;

	int port() const
	{
		return m_port;
	}

	void setPort( int port )
	{
		m_port = port;
	}

	std::string password() const
	{
		return m_password;
	}

	void setPassword( const std::string& password )
	{
		m_password = password;
	}

	Clipboard* clipboard() const
	{
		return m_clipboard;
	}

	Framebuffer* framebuffer() const
	{
		return m_framebuffer;
	}

	Keyboard* keyboard() const
	{
		return m_keyboard;
	}

	PointingDevice* pointingDevice() const
	{
		return m_pointingDevice;
	}

	bool run();
	void quit();

private:
	static constexpr auto IdleTimeout = 100;
	static constexpr auto NonIdleTimeout = 5;

	bool createFramebuffer();
	bool createKeyboard();
	bool createPointingDevice();
	bool createClipboard();
	bool createBackend();

	void shutdown();

	int m_port{5900};
	std::string m_password{};

	std::atomic<bool> m_quit{false};

	Framebuffer* m_framebuffer{nullptr};
	Keyboard* m_keyboard{nullptr};
	PointingDevice* m_pointingDevice{nullptr};
	Clipboard* m_clipboard{nullptr};
	Backend* m_backend{nullptr};

};

}

}
