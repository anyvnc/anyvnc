include(AnyVnc)

add_anyvnc_plugin(backend-libvncserver
	LibVncServerBackend.cpp
	LibVncServerBackend.h
)

target_link_libraries(backend-libvncserver LibVNC::LibVNCServer)
