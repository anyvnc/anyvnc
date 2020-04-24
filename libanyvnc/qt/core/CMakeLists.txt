include(AnyVnc)

set(CMAKE_AUTOMOC TRUE)
set(CMAKE_AUTOUIC TRUE)
set(CMAKE_AUTORCC TRUE)

add_anyvnc_library(anyvnc-qt-core
	AnyVncQtCore.h
	AnyVncQt.h
	AnyVncQt.cpp
	KeyboardShortcutTrapper.h
	VncConnection.h
	VncConnection.cpp
	VncEvents.h
	VncEvents.cpp
	VncServer.h
	VncServer.cpp
	VncView.h
	VncView.cpp
)

target_link_libraries(anyvnc-qt-core anyvnc-core Qt5::Concurrent Qt5::Gui)

# TODO: decouple from libvncclient through ClientBackend plugin
find_package(LibVNCClient 0.9.13 REQUIRED)
target_link_libraries(anyvnc-qt-core LibVNC::LibVNCClient)
