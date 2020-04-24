include(AnyVnc)

if(Qt5AndroidExtras_FOUND)

add_anyvnc_plugin(pointingdevice-android
	AndroidPointingDevice.cpp
	AndroidPointingDevice.h
)

target_link_libraries(pointingdevice-android Qt5::AndroidExtras)

endif()
