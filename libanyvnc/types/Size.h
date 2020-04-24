/*
 * types/Size.h
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

class Size
{
public:
	Size( int width = -1, int height = -1) :
		m_width( width ),
		m_height( height )
	{
	}

	bool operator==( Size other ) const
	{
		return width() == other.width() &&
			   height() == other.height();
	}

	bool operator!=( Size other ) const
	{
		return !( *this == other );
	}

	bool isValid() const
	{
		return m_width >= 0 && m_height >= 0;
	}

	bool isNull() const
	{
		return m_width * m_height == 0;
	}

	int width() const
	{
		return m_width;
	}

	int height() const
	{
		return m_height;
	}

private:
	int m_width;
	int m_height;

};

}

}

