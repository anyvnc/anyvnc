/*
 * DummyFramebuffer.h
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

#include "libanyvnc/interfaces/Framebuffer.h"

namespace AnyVnc
{

// clazy:excludeall=copyable-polymorphic

class DummyFramebuffer : public Interfaces::Framebuffer
{
public:
	explicit DummyFramebuffer() = default;

	std::string uid() const override
	{
		return "379e2a94-767f-4785-b6bc-63064a43b8b0";
	}

	Types::VersionNumber version() const override
	{
		return { 1, 0 };
	}

	std::string name() const override
	{
		return "DummyFramebuffer";
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

	void* data() const override;
	Types::Size size() const override;

	UpdateFlags update( const RectangleVisitor& visitor ) override;

	Types::Screens availableScreens() const override;

private:
	static constexpr auto DummyResolutionX = 100;
	static constexpr auto DummyResolutionY = 100;

	std::array<uint32_t, DummyResolutionX*DummyResolutionY> m_dummyFramebuffer{};
	void* m_framebufferData{m_dummyFramebuffer.data()};
};

}
