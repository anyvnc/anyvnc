/*
 * WindowsDeskDupEngineFramebuffer.cpp
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

#include <iostream>

#include "WindowsDeskDupEngineFramebuffer.h"
#include "DeskDupEngine.h"


namespace AnyVnc
{

WindowsDeskDupEngineFramebuffer::~WindowsDeskDupEngineFramebuffer()
{
	delete m_deskDupEngine;
}


bool WindowsDeskDupEngineFramebuffer::initialize( Core::Server* server )
{
	m_server = server;

	m_deskDupEngine = new DeskDupEngine;

	return m_deskDupEngine->start( false );
}



void* WindowsDeskDupEngineFramebuffer::data() const
{
	if( m_deskDupEngine )
	{
		return m_deskDupEngine->framebuffer();
	}

	return nullptr;
}



Types::Size WindowsDeskDupEngineFramebuffer::size() const
{
	if( m_deskDupEngine )
	{
		return m_deskDupEngine->currentVirtualScreenGeometry();
	}

	return {};
}


static void visitChangeRecord( const Interfaces::Framebuffer::RectangleVisitor& visitor,
					   const DeskDupEngine::ChangesRecord& changesRecord )
{
	const auto& rect = changesRecord.rect;

	switch( DeskDupEngine::Change(changesRecord.type) )
	{
	case DeskDupEngine::Change::ScreenToScreen:
		// TODO
		std::cout << "ScreenToScreen" << rect.left << rect.top << rect.right << rect.bottom;
		break;
	case DeskDupEngine::Change::SolidFill:
	case DeskDupEngine::Change::Textout:
	case DeskDupEngine::Change::Blend:
	case DeskDupEngine::Change::Trans:
	case DeskDupEngine::Change::Plg:
	case DeskDupEngine::Change::Blit:
		visitor( { rect.left, rect.top, rect.right, rect.bottom } );
		break;
	case DeskDupEngine::Change::Pointer:
		// TODO
		std::cout << "Pointer" << rect.left << rect.top << rect.right << rect.bottom;
		break;
	case DeskDupEngine::Change::Unknown:
		std::cout << "unknown DeskDupEngine change type" << changesRecord.type;
		break;
	}
}



WindowsDeskDupEngineFramebuffer::UpdateFlags WindowsDeskDupEngineFramebuffer::update( const RectangleVisitor& visitor )
{
	const auto previousCounter = m_deskDupEngine->previousCounter();
	auto counter = m_deskDupEngine->changesBuffer()->counter;

	if( previousCounter == counter ||
		counter < 1 ||
		counter > DeskDupEngine::MaxChanges )
	{
		return UpdateFlags{ UpdateFlag::None };
	}

	const auto* changes = m_deskDupEngine->changesBuffer()->changes;

	if( previousCounter < counter )
	{
		for( ULONG i = previousCounter+1; i <= counter; ++i )
		{
			visitChangeRecord( visitor, changes[i] );
		}
	}
	else
	{
		for( ULONG i = previousCounter + 1; i < DeskDupEngine::MaxChanges; ++i )
		{
			visitChangeRecord( visitor, changes[i] );
		}

		for( ULONG i = 1; i <= counter; ++i )
		{
			visitChangeRecord( visitor, changes[i] );
		}
	}

	m_deskDupEngine->setPreviousCounter( counter );

	return UpdateFlags{ UpdateFlag::None };
}



Types::Screens WindowsDeskDupEngineFramebuffer::availableScreens() const
{
	// TODO: multi monitor support
	return { Types::Screen { size(), 32 } };
}

}

ANYVNC_EXPORT_PLUGIN(AnyVnc::WindowsDeskDupEngineFramebuffer)
