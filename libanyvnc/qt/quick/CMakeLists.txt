include(AnyVnc)

add_anyvnc_library(anyvnc-qt-quick
	AnyVncQtQuick.h
	QSGImageTexture.h
	QSGImageTexture.cpp
	VncViewItemBase.h
	VncViewItemBase.cpp
	VncViewItem.h
	VncViewItem.cpp
)

target_link_libraries(anyvnc-qt-quick anyvnc-qt-core Qt5::Quick)

if(ANDROID)
	target_link_libraries(anyvnc-qt-quick anyvnc-qt-core -lGLESv2)
endif()
