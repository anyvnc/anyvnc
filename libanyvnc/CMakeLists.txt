include(AnyVnc)

add_subdirectory(core)
add_subdirectory(interfaces)

if(Qt5Gui_FOUND)
add_subdirectory(qt)
endif()
