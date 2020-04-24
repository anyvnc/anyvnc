include(AnyVnc)

if(Qt5AndroidExtras_FOUND)

add_anyvnc_plugin(framebuffer-android-mediaprojection
	AndroidMediaProjectionFramebuffer.cpp
	AndroidMediaProjectionFramebuffer.h
)

target_link_libraries(framebuffer-android-mediaprojection anyvnc-qt-core Qt5::AndroidExtras)

find_package(Java 1.8 COMPONENTS Development)
include(UseJava)
set(autojs_DIR ${CMAKE_CURRENT_SOURCE_DIR}/auto.js)

add_jar(AnyVncVncMediaProjection SOURCES
	${autojs_DIR}/common/src/main/java/com/stardust/lang/ThreadCompat.java
	${autojs_DIR}/common/src/main/java/com/stardust/util/ScreenMetrics.java
	${autojs_DIR}/autojs/src/main/java/com/stardust/autojs/runtime/exception/ScriptException.java
	${autojs_DIR}/autojs/src/main/java/com/stardust/autojs/core/image/capture/ScreenCapturer.java
	${autojs_DIR}/autojs/src/main/java/com/stardust/autojs/runtime/exception/ScriptInterruptedException.java
	INCLUDE_JARS ${ANDROID_SDK}/platforms/android-${ANDROID_NATIVE_API_LEVEL}/android.jar
)

install_jar(AnyVncVncMediaProjection "jar")

endif()
