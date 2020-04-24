/*
 * interfaces/ServerBackend.h
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

#include "Plugin.h"

namespace AnyVnc
{

namespace Core
{
class Server;
}

namespace Interfaces
{

// clazy:excludeall=copyable-polymorphic

class ANYVNC_INTERFACES_EXPORT ServerBackend : public Plugin
{
public:
	~ServerBackend() override;

	virtual bool initialize( Core::Server* server ) = 0;
	virtual bool handleFramebufferUpdate() = 0;
	virtual bool hasConnectedClients() const = 0;
	virtual bool hasPendingClientUpdateRequests() const = 0;
	virtual bool processEvents( int timeout ) = 0;
	virtual bool shutdown() = 0;

};

}

}

