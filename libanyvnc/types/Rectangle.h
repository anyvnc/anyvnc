/*
 * types/Rectangle.h
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

class Rectangle
{
public:
	Rectangle( int left = -1, int top = -1, int right = -1, int bottom = -1 ) :
		m_left( left ),
		m_top( top ),
		m_right( right ),
		m_bottom( bottom )
	{
	}

	bool isValid() const
	{
		return m_left >= 0 && m_right >= 0 && m_top >= 0 && m_bottom >= 0;
	}

	int left() const
	{
		return m_left;
	}

	int top() const
	{
		return m_top;
	}

	int right() const
	{
		return m_right;
	}

	int bottom() const
	{
		return m_bottom;
	}

	void setLeft( int left )
	{
		m_left = left;
	}

	void setTop( int top )
	{
		m_top = top;
	}

	void setRight( int right )
	{
		m_right = right;
	}

	void setBottom( int bottom )
	{
		m_bottom = bottom;
	}

private:
	int m_left;
	int m_top;
	int m_right;
	int m_bottom;

};

}

}

