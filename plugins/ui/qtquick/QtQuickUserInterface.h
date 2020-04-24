/*
 * QtQuickUserInterface.h
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

#include "libanyvnc/interfaces/UserInterface.h"

namespace AnyVnc
{

// clazy:excludeall=copyable-polymorphic

class QtQuickUserInterface : public Interfaces::UserInterface
{
public:
	explicit QtQuickUserInterface() = default;

	std::string uid() const override
	{
		return "cc9c3a30-a315-474d-908a-bce1ba106ad4";
	}

	Types::VersionNumber version() const override
	{
		return { 1, 0 };
	}

	std::string name() const override
	{
		return "QtQuickUserInterface";
	}

	std::string description() const override
	{
		return "QtQuick user interface";
	}

	std::string vendor() const override
	{
		return "AnyVNC Community";
	}

	std::string copyright() const override
	{
		return "Tobias Junghans";
	}

	bool initialize() override;
	bool run( int argc, char** argv ) override;

};

}
