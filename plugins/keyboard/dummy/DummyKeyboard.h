/*
 * DummyKeyboard.h
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

#include "libanyvnc/interfaces/Keyboard.h"

namespace AnyVnc
{

// clazy:excludeall=copyable-polymorphic

class DummyKeyboard : public Interfaces::Keyboard
{
public:
	explicit DummyKeyboard() = default;

	std::string uid() const override
	{
		return "a16ca339-7495-4de0-8e84-affa7e861ff0";
	}

	Types::VersionNumber version() const override
	{
		return { 1, 0 };
	}

	std::string name() const override
	{
		return "DummyKeyboard";
	}

	std::string description() const override
	{
		return "Dummy pointing device";
	}

	std::string vendor() const override
	{
		return "AnyVNC Community";
	}

	std::string copyright() const override
	{
		return "Tobias Junghans";
	}

	bool initialize( Core::Server* server ) override;

	void synthesizeKeyEvent( KeySym keySym, bool down ) override;

};

}
