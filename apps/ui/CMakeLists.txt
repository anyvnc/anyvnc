include(AnyVnc)

add_anyvnc_executable(anyvnc-ui main.cpp
	android/AndroidManifest.xml)

if(ANDROID)
	# make androiddeployqt add libraries to APK implicitely by linking
	target_link_libraries(anyvnc-ui anyvnc-qt-quick)
	target_link_libraries(anyvnc-ui Qt5::AndroidExtras)
endif()
