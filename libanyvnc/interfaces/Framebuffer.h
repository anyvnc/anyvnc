/*
 * interfaces/Framebuffer.h
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

#include <functional>
#include <vector>

#include "libanyvnc/types/Rectangle.h"
#include "libanyvnc/types/Size.h"
#include "libanyvnc/types/Screen.h"

#include "ServerPlugin.h"

namespace AnyVnc
{

namespace Interfaces
{

// clazy:excludeall=copyable-polymorphic

class ANYVNC_INTERFACES_EXPORT Framebuffer : public ServerPlugin
{
public:
	enum class UpdateFlag
	{
		None,
		Initializing = 0x0001,
		SizeChanged = 0x0002,
		RequiresRestart = 0x0004,
		_
	} ;
	using UpdateFlags = flag_set<UpdateFlag>;

	using RectangleVisitor = std::function<void(Types::Rectangle)>;

	~Framebuffer() override;

	virtual void* data() const = 0;
	virtual Types::Size size() const = 0;

	virtual UpdateFlags update( const RectangleVisitor& visitor ) = 0;

	virtual Types::Screens availableScreens() const = 0;

};

}

}
