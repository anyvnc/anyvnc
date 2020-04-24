/*
 * interfaces/Plugin.h - interface class for plugins
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

#include <string>
#include <vector>

#include "AnyVncInterfaces.h"
#include "libanyvnc/types/FlagSet.h"
#include "libanyvnc/types/VersionNumber.h"

namespace AnyVnc
{

namespace Interfaces
{

// clazy:excludeall=copyable-polymorphic

class ANYVNC_INTERFACES_EXPORT Plugin
{
public:
	enum class Flag
	{
		None,
		ProvidesDefaultImplementation = 0x0001,
		_
	} ;
	using Flags = flag_set<Flag>;

	virtual ~Plugin();

	virtual std::string uid() const = 0;
	virtual Types::VersionNumber version() const = 0;
	virtual std::string name() const = 0;
	virtual std::string description() const = 0;
	virtual std::string vendor() const = 0;
	virtual std::string copyright() const = 0;
	virtual Flags flags() const
	{
		return Flags{ Flag::None };
	}

};

using Plugins = std::vector<Plugin *>;

}

}

#define ANYVNC_EXPORT_PLUGIN(name) \
	extern "C" ANYVNC_DECL_EXPORT AnyVnc::Interfaces::Plugin* createAnyVncPluginInstance() { return new name; }
