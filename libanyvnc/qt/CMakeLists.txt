add_subdirectory(core)

if(Qt5Widgets_FOUND)
add_subdirectory(widgets)
endif()

if(Qt5Quick_FOUND)
add_subdirectory(quick)
endif()
