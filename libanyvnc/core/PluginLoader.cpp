/*
 * PluginLoader.cpp
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

#include <array>
#include <iostream>

#ifdef QT_CORE_LIB
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QLibrary>
#elif defined(WIN32)
#include <windows.h>
#elif defined(__unix__)
#include <dlfcn.h>
#include <climits>
#include <unistd.h>
#endif

#include "PluginLoader.h"
#include "Utils.h"

namespace AnyVnc
{

namespace Core
{

filesystem::path PluginLoader::applicationFilePath()
{
#ifdef QT_CORE_LIB
	return QCoreApplication::applicationFilePath();
#elif defined(WIN32)
	std::array<wchar_t, MAX_PATH> path{};
	GetModuleFileNameW( nullptr, path.data(), MAX_PATH );
	return { path.data() };
#else
	std::array<char, PATH_MAX> path{};
	const auto count = size_t(readlink("/proc/self/exe", path.data(), PATH_MAX));
	return std::string( path.data(), (count > 0) ? count : 0 );
#endif
}



filesystem::path PluginLoader::applicationDirPath()
{
#ifdef QT_CORE_LIB
	return QCoreApplication::applicationDirPath();
#else
	return applicationFilePath().remove_filename();
#endif
}



filesystem::path PluginLoader::pluginPath()
{
#ifdef QT_CORE_LIB
	return QStringLiteral(ANYVNC_PLUGIN_DIR);
#else
	return ANYVNC_PLUGIN_DIR;
#endif
}



std::string PluginLoader::pluginSuffix()
{
#ifdef __unix__
	return ".so";
#elif defined(WIN32)
	return ".dll";
#else
#error platform not supported
#endif
}



const char* PluginLoader::pluginEntryPoint()
{
	return "createAnyVncPluginInstance";
}



Interfaces::Plugin* PluginLoader::createInstance( const PluginTypeQualifier& qualifier )
{
#ifdef QT_CORE_LIB
	for( const auto& entry : QDir( applicationDirPath() + QDir::separator() + pluginPath() ).entryInfoList() )
	{
		// skip simple shared libraries
		if( entry.fileName().startsWith( QLatin1String("lib") ) )
		{
			continue;
		}

		if( entry.fileName().endsWith( QString::fromStdString( pluginSuffix() ) ) )
		{
			auto instance = loadPlugin( entry.filePath(), qualifier );
			if( instance )
			{
				return instance;
			}
		}
	}
#else
	for( const auto& entry : filesystem::directory_iterator( applicationDirPath() / pluginPath() ) )
	{
		if( Utils::ends_with( entry.path().string(), pluginSuffix() ) )
		{
			auto instance = loadPlugin( entry.path().string(), qualifier );
			if( instance )
			{
				return instance;
			}

		}
	}
#endif

	return nullptr;
}



Interfaces::Plugin* PluginLoader::loadPlugin( const filesystem::path& path, const PluginTypeQualifier& qualifier )
{
#ifdef QT_CORE_LIB
	QLibrary plugin( path );
	if( plugin.load() == false )
	{
		qCritical() << "failed to load plugin" << path;
		return nullptr;
	}

	auto instance = CreatePluginInstance( plugin.resolve( pluginEntryPoint() ) )();
#elif defined(__unix__)
	auto libraryHandle = ::dlopen( path.c_str(), uint(RTLD_NOW) | uint(RTLD_NODELETE) );
	if( libraryHandle == nullptr )
	{
		std::cerr << "failed to load plugin" << path << std::endl;
		return nullptr;
	}

	auto instance = CreatePluginInstance( dlsym(libraryHandle, pluginEntryPoint()) )();

	dlclose( libraryHandle );
#elif defined(WIN32)
	auto libraryHandle = LoadLibraryW( path.c_str() );
	if( libraryHandle == nullptr )
	{
		std::cerr << "failed to load plugin:" << path << std::endl;
		return nullptr;
	}

	auto instance = CreatePluginInstance( GetProcAddress(libraryHandle, pluginEntryPoint()) )();
	CloseHandle( libraryHandle );
#else
#error platform not supported
#endif

	if( qualifier( instance ) )
	{
		return instance;
	}

	return nullptr;
}

}

}
