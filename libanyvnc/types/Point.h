/*
 * types/Point.h
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

namespace AnyVnc
{

namespace Types
{

class Point
{
public:
	Point( int x = 0, int y = 0 ) :
		m_x( x ),
		m_y( y )
	{
	}

	bool isNull() const
	{
		return m_x == 0 || m_y == 0;
	}

	int x() const
	{
		return m_x;
	}

	int y() const
	{
		return m_y;
	}

private:
	int m_x{0};
	int m_y{0};

};

}

}

