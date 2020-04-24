include(AnyVnc)

add_anyvnc_library(anyvnc-core
	AnyVncCore.h
	PluginLoader.h
	PluginLoader.cpp
	Server.h
	Server.cpp
	Export.h
	Utils.h
)

target_link_libraries(anyvnc-core anyvnc-interfaces)
target_include_directories(anyvnc-core PUBLIC ${CMAKE_SOURCE_DIR})
target_compile_definitions(anyvnc-core PRIVATE ANYVNC_PLUGIN_DIR="${ANYVNC_PLUGIN_DIR}")

if(CMAKE_COMPILER_IS_GNUCC)
	if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
		target_link_libraries(anyvnc-core stdc++fs)
	endif()
endif()

if(ANDROID)
	target_link_libraries(anyvnc-core Qt5::Core)
elseif(UNIX)
	target_link_libraries(anyvnc-core dl)
endif()
