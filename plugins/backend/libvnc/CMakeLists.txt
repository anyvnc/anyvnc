find_package(LibVNCClient)
find_package(LibVNCServer)

if(LibVNCServer_FOUND)
add_subdirectory(server)
endif()
