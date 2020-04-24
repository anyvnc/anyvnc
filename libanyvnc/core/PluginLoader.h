/*
 * PluginLoader.h
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

#include <functional>
#include "libanyvnc/core/AnyVncCore.h"
#include "libanyvnc/core/Filesystem.h"
#include "libanyvnc/interfaces/Plugin.h"

namespace AnyVnc
{

namespace Core
{

class ANYVNC_CORE_EXPORT PluginLoader
{
public:
	PluginLoader() = default;

	template<class T>
	static T* create( const std::string& uid = {} )
	{
		const auto checkInstanceType = []( auto instance ) -> bool { return dynamic_cast<T *>( instance ) != nullptr; };

		for( const auto& qualifier : std::initializer_list<PluginTypeQualifier> {
				 [&checkInstanceType, &uid]( auto instance ) { return checkInstanceType( instance ) && instance->uid() == uid; },
				 [&checkInstanceType]( auto instance ) { return checkInstanceType( instance ) && instance->flags() & Interfaces::Plugin::Flag::ProvidesDefaultImplementation; },
				 checkInstanceType
			 } )
		{
			auto instance = dynamic_cast<T *>( createInstance( qualifier ) );
			if( instance )
			{
				return instance;
			}
		}

		return nullptr;
	}

	template<class T, class C>
	static T* createAndInitialize( C* component )
	{
		T* instance = create<T>();
		if( instance && instance->initialize( component ) )
		{
			return instance;
		}

		return nullptr;
	}

	static filesystem::path applicationFilePath();
	static filesystem::path applicationDirPath();

private:
	using PluginTypeQualifier = std::function<bool(Interfaces::Plugin *)>;
	using CreatePluginInstance = AnyVnc::Interfaces::Plugin*(*)();

	static filesystem::path pluginPath();
	static std::string pluginSuffix();
	static const char* pluginEntryPoint();

	static Interfaces::Plugin* createInstance( const PluginTypeQualifier& qualifier );
	static Interfaces::Plugin* loadPlugin( const filesystem::path& path, const PluginTypeQualifier& qualifier );

};

}

}
