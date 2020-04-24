add_subdirectory(dummy)

if(WIN32)
add_subdirectory(windows)
endif()

if(ANDROID)
add_subdirectory(android)
endif()
