include(AnyVnc)

add_anyvnc_plugin(ui-qtquick
	QtQuickUserInterface.h
	QtQuickUserInterface.cpp
	qtquick-ui.qrc
)

target_link_libraries(ui-qtquick
	anyvnc-qt-quick
	Qt5::QuickControls2
)
