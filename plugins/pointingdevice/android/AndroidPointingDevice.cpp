/*
 * AndroidPointingDevice.cpp
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


#include <QAndroidJniObject>

#include "AndroidPointingDevice.h"

#include "libanyvnc/core/Server.h"

namespace AnyVnc
{

bool AndroidPointingDevice::initialize( Core::Server* server )
{
	Q_UNUSED(server)

	return true;
}



void AndroidPointingDevice::move( Types::Point pos )
{
	m_pos = pos;
}



void AndroidPointingDevice::pressButton( Button button )
{
	switch( button )
	{
	case Button::Left: QAndroidJniObject::callStaticMethod<void>( "com/anyvnc/AnyVnc/AnyVncAccessibilityService", "tap", "(II)V", m_pos.x(), m_pos.y() ); break;
	case Button::Right: QAndroidJniObject::callStaticMethod<void>( "com/anyvnc/AnyVnc/AnyVncAccessibilityService", "longPress", "(II)V", m_pos.x(), m_pos.y() ); break;
	default: break;
	}

}



void AndroidPointingDevice::releaseButton( Button )
{
}



void AndroidPointingDevice::scrollUp()
{
	QAndroidJniObject::callStaticMethod<void>( "com/anyvnc/AnyVnc/AnyVncAccessibilityService", "swipeDown", "(II)V", m_pos.x(), m_pos.y() );
}



void AndroidPointingDevice::scrollDown()
{
	QAndroidJniObject::callStaticMethod<void>( "com/anyvnc/AnyVnc/AnyVncAccessibilityService", "swipeUp", "(II)V", m_pos.x(), m_pos.y() );
}


}

ANYVNC_EXPORT_PLUGIN(AnyVnc::AndroidPointingDevice)
