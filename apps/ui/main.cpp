/*
 * main.cpp - main file for AnyVNC UI
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

#ifdef ANYVNC_OS_ANDROID
#include <QGuiApplication>
#endif

#include "libanyvnc/core/PluginLoader.h"
#include "libanyvnc/interfaces/UserInterface.h"

ANYVNC_DECL_EXPORT int main( int argc, char **argv )
{
#ifdef ANYVNC_OS_ANDROID
	QGuiApplication app( argc, argv );
#endif

	auto userInterface = AnyVnc::Core::PluginLoader::create<AnyVnc::Interfaces::UserInterface>();
	if( userInterface )
	{
		return userInterface->run( argc, argv );
	}

	return -1;
}
