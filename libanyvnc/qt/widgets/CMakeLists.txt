include(AnyVnc)

add_anyvnc_library(anyvnc-qt-widgets
	AnyVncQtWidgets.h
	VncViewWidget.h
	VncViewWidget.cpp
)

target_link_libraries(anyvnc-qt-widgets anyvnc-qt-core Qt5::Widgets)
