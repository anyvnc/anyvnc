/*
 * LibVncServerBackend.h
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

#pragma once

#include <array>

extern "C" {
#include <rfb/rfb.h>
}

#include "libanyvnc/interfaces/ServerBackend.h"

namespace AnyVnc
{

// clazy:excludeall=copyable-polymorphic

class LibVncServerBackend : public Interfaces::ServerBackend
{
public:
	explicit LibVncServerBackend() = default;
	~LibVncServerBackend() override;

	std::string uid() const override
	{
		return "3cf3480f-9ea4-4940-a48d-ca1425052acf";
	}

	Types::VersionNumber version() const override
	{
		return { 1, 0 };
	}

	std::string name() const override
	{
		return "LibVncServerBackend";
	}

	std::string description() const override
	{
		return "libvncserver backend";
	}

	std::string vendor() const override
	{
		return "AnyVNC Community";
	}

	std::string copyright() const override
	{
		return "Tobias Junghans";
	}

	Flags flags() const override
	{
		return Flags{ Flag::ProvidesDefaultImplementation };
	}

	bool initialize( Core::Server* server ) override;

	bool handleFramebufferUpdate() override;
	bool hasConnectedClients() const override;
	bool hasPendingClientUpdateRequests() const override;
	bool processEvents( int timeout ) override;
	bool shutdown() override;

private:
	static constexpr auto MicroSecondsPerMilliSecond = 1000;

	Core::Server* m_server{nullptr};
	rfbScreenInfoPtr m_rfbScreen{nullptr};
	std::string m_password;
	std::array<const char *, 2> m_passwords{};

};

}
