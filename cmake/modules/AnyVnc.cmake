# AnyVncBuldFunctions.cmake - Copyright (c) 2020 Tobias Junghans
#

macro(set_default_target_properties TARGET_NAME)
	set_target_properties(${TARGET_NAME} PROPERTIES
		CXX_STANDARD 17
		CXX_VISIBILITY_PRESET hidden
		VISIBILITY_INLINES_HIDDEN 1
		CXX_STANDARD_REQUIRED ON
	)
	if(NOT DISABLE_LTO)
		#set_target_properties(${TARGET_NAME} PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
	endif()
	target_compile_definitions(${TARGET_NAME} PRIVATE _FORTIFY_SOURCE=2)
endmacro()

macro(add_anyvnc_library LIBRARY_NAME)
	add_library(${LIBRARY_NAME} SHARED ${ARGN})

	target_include_directories(${LIBRARY_NAME} PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
	target_compile_options(${LIBRARY_NAME} PRIVATE ${ANYVNC_COMPILE_OPTIONS})

	set_default_target_properties(${LIBRARY_NAME})
	set_target_properties(${LIBRARY_NAME} PROPERTIES LINK_FLAGS "-Wl,-no-undefined")
	install(TARGETS ${LIBRARY_NAME} DESTINATION ${ANYVNC_LIB_DIR})
endmacro()


macro(add_anyvnc_plugin PLUGIN_NAME)
	add_library(${PLUGIN_NAME} MODULE ${ARGN})

	target_include_directories(${PLUGIN_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
	target_link_libraries(${PLUGIN_NAME} anyvnc-core)
	target_compile_options(${PLUGIN_NAME} PRIVATE ${ANYVNC_COMPILE_OPTIONS})

	set_default_target_properties(${PLUGIN_NAME})
	set_target_properties(${PLUGIN_NAME} PROPERTIES PREFIX "")
	set_target_properties(${PLUGIN_NAME} PROPERTIES LINK_FLAGS "-Wl,-no-undefined")
	install(TARGETS ${PLUGIN_NAME} LIBRARY DESTINATION ${ANYVNC_INSTALL_PLUGIN_DIR})
endmacro()

macro(add_anyvnc_executable EXECUTABLE_NAME)
	if(ANDROID)
		add_library(${EXECUTABLE_NAME} SHARED ${ARGN})
	else()
		add_executable(${EXECUTABLE_NAME} ${ARGN})
		install(TARGETS ${EXECUTABLE_NAME} RUNTIME DESTINATION bin)
	endif()

	target_link_libraries(${EXECUTABLE_NAME} anyvnc-core)
	target_compile_options(${EXECUTABLE_NAME} PRIVATE ${ANYVNC_COMPILE_OPTIONS})

	set_default_target_properties(${EXECUTABLE_NAME})
endmacro()

