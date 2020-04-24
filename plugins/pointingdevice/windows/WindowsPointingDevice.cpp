/*
 * WindowsPointingDevice.cpp
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

#include <windows.h>

#include "WindowsPointingDevice.h"

#include "libanyvnc/core/Server.h"
#include "libanyvnc/interfaces/Framebuffer.h"

namespace AnyVnc
{

bool WindowsPointingDevice::initialize( Core::Server* server )
{
	m_server = server;

	return true;
}



void WindowsPointingDevice::move( Types::Point pos )
{
	m_pos = pos;

	mouseEvent( MOUSEEVENTF_MOVE );
}


void WindowsPointingDevice::pressButton( Button button )
{
	if( GetSystemMetrics(SM_SWAPBUTTON) )
	{
		switch(button)
		{
		case Button::Left: button = Button::Right; break;
		case Button::Right: button = Button::Left; break;
		case Button::Middle: break;
		}
	}

	switch( button )
	{
	case Button::Left: mouseEvent( MOUSEEVENTF_LEFTDOWN ); break;
	case Button::Middle: mouseEvent( MOUSEEVENTF_MIDDLEDOWN ); break;
	case Button::Right: mouseEvent( MOUSEEVENTF_RIGHTDOWN ); break;
	}

}

void WindowsPointingDevice::releaseButton( Button button )
{
	if( GetSystemMetrics(SM_SWAPBUTTON) )
	{
		switch(button)
		{
		case Button::Left: button = Button::Right; break;
		case Button::Right: button = Button::Left; break;
		case Button::Middle: break;
		}
	}

	switch( button )
	{
	case Button::Left: mouseEvent( MOUSEEVENTF_LEFTUP ); break;
	case Button::Middle: mouseEvent( MOUSEEVENTF_MIDDLEUP ); break;
	case Button::Right: mouseEvent( MOUSEEVENTF_RIGHTUP ); break;
	}
}



void WindowsPointingDevice::scrollUp()
{
	mouseEvent( MOUSEEVENTF_WHEEL, WHEEL_DELTA );
}



void WindowsPointingDevice::scrollDown()
{
	mouseEvent( MOUSEEVENTF_WHEEL, -WHEEL_DELTA );
}



void WindowsPointingDevice::mouseEvent( int flags, int wheelDelta )
{
	// TODO: multi-monitor support
	auto screen = m_server->framebuffer()->availableScreens().at(0);

	::mouse_event( DWORD(MOUSEEVENTF_ABSOLUTE | flags),
				   DWORD(m_pos.x() * 65535 / screen.geometry().width()),
				   DWORD(m_pos.y() * 65535 / screen.geometry().height()),
				   DWORD(wheelDelta),
				   0 );
}

}

ANYVNC_EXPORT_PLUGIN(AnyVnc::WindowsPointingDevice)
