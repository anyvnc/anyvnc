/*
 * WindowsPointingDevice.h
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

#include "libanyvnc/interfaces/PointingDevice.h"

namespace AnyVnc
{

class WindowsPointingDevice : public Interfaces::PointingDevice
{
public:
	explicit WindowsPointingDevice() = default;

	std::string uid() const override
	{
		return "9ec82f32-3cde-41f6-b864-bef39b43bf48";
	}

	Types::VersionNumber version() const override
	{
		return { 1, 0 };
	}

	std::string name() const override
	{
		return "WindowsPointingDevice";
	}

	std::string description() const override
	{
		return "Windows default pointing device";
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
	void move( Types::Point pos ) override;
	void pressButton( Button button ) override;
	void releaseButton( Button button ) override;
	void scrollUp() override;
	void scrollDown() override;

private:
	void mouseEvent( int flags, int wheelDelta = 0 );

	Core::Server* m_server{nullptr};
	Types::Point m_pos{};

};

}
