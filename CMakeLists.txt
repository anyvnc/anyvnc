cmake_minimum_required(VERSION 3.1)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE relwithdebinfo)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

if(COMMAND CMAKE_POLICY)
	cmake_policy(SET CMP0063 NEW)
	if(${CMAKE_VERSION} VERSION_GREATER "3.8.0")
		cmake_policy(SET CMP0069 NEW)
	endif()
endif()

include(GNUInstallDirs)

find_package(Git)

if(GIT_FOUND)
	execute_process(COMMAND "${GIT_EXECUTABLE}" describe --tags
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		OUTPUT_STRIP_TRAILING_WHITESPACE
		OUTPUT_VARIABLE VERSION_STRING)
	string(REGEX REPLACE "^v([0-9]+)\\..*" "\\1" VERSION_MAJOR "${VERSION_STRING}")
	string(REGEX REPLACE "^v[0-9]+\\.([0-9]+).*" "\\1" VERSION_MINOR "${VERSION_STRING}")
	string(REGEX REPLACE "^v[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" VERSION_PATCH "${VERSION_STRING}")

	# determine build number to use in NSIS installer and resource files
	execute_process(COMMAND "${GIT_EXECUTABLE}" describe --tags
		COMMAND cut -d "-" -f2
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		OUTPUT_STRIP_TRAILING_WHITESPACE
		OUTPUT_VARIABLE VERSION_BUILD)
	if(NOT VERSION_BUILD GREATER 0)
		set(VERSION_BUILD 0)
	endif()
endif()

# can't retrieve version information as not building from Git repository?
if(NOT VERSION_STRING)
	set(VERSION_MAJOR 0)
	set(VERSION_MINOR 1)
	set(VERSION_PATCH 0)
	set(VERSION_BUILD 0)
	set(VERSION_STRING "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
else()
	# remove leading character from tag name
	string(REPLACE "v" "" VERSION_STRING "${VERSION_STRING}")
endif()

project(anyvnc VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

string(REGEX REPLACE "\\.[0-9]$" "" COMPILER_VERSION_MAJOR_MINOR ${CMAKE_CXX_COMPILER_VERSION})

if(WIN32)
	if(WIN64)
		add_definitions(-DANYVNC_OS_WIN64)
	else()
		add_definitions(-DANYVNC_OS_WIN32)
	endif()
endif()
if(APPLE)
	add_definitions(-DANYVNC_OS_OSX)
endif()
if(UNIX AND NOT ANDROID)
	add_definitions(-DANYVNC_OS_LINUX)
endif()
if(ANDROID)
	add_definitions(-DANYVNC_OS_ANDROID)
endif()


# set up library and plugin path variables
if(ANDROID)
	set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)
	set(ANYVNC_LIB_DIR "lib")
	set(ANYVNC_INSTALL_PLUGIN_DIR "${ANYVNC_LIB_DIR}/anyvnc")
	set(ANYVNC_INSTALL_DATA_DIR "${CMAKE_INSTALL_DATADIR}/anyvnc")
	set(ANYVNC_PLUGIN_DIR "")
	set(ANYVNC_TRANSLATIONS_DIR "/translations")

	if(ANDROID_BUILD_ABI_armeabi-v7a)
		set(ANDROID_TOOLCHAIN_MACHINE_NAME "arm-linux-androideabi")
	elseif(ANDROID_BUILD_ABI_arm64-v8a)
		set(ANDROID_TOOLCHAIN_MACHINE_NAME "aarch64-linux-android")
	endif()

	set(QT_ANDROID_APPLICATION_BINARY "anyvnc-ui")
	set(ANDROID_NATIVE_API_LEVEL 29)

	set(ANDROID_LIBS_ROOT "/usr/${ANDROID_TOOLCHAIN_MACHINE_NAME}/lib")
	list(APPEND CMAKE_FIND_ROOT_PATH "${ANDROID_LIBS_ROOT}")

	set(ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_SOURCE_DIR}/apps/ui/android)
	set(ANDROID_INSTALL_DIR "${CMAKE_BINARY_DIR}/install")
	set(ANDROID_EXTRA_PLUGINS ${ANDROID_INSTALL_DIR}/${ANYVNC_LIB_DIR}/anyvnc ${ANDROID_INSTALL_DIR}/jar)
	list(APPEND ANDROID_EXTRA_LIBS "${ANDROID_LIBS_ROOT}/libvncserver.so")
	list(APPEND ANDROID_EXTRA_LIBS "${ANDROID_LIBS_ROOT}/libvncclient.so")
	list(APPEND ANDROID_EXTRA_LIBS "${ANDROID_LIBS_ROOT}/libsasl2.so")
	list(APPEND ANDROID_EXTRA_LIBS "${ANDROID_LIBS_ROOT}/liblzo2.so")
	list(APPEND ANDROID_EXTRA_LIBS "${ANDROID_LIBS_ROOT}/libjpeg.so")
	list(APPEND ANDROID_EXTRA_LIBS "${ANDROID_LIBS_ROOT}/libpng16.so")
	list(APPEND ANDROID_EXTRA_LIBS "${ANDROID_LIBS_ROOT}/libssl.so")
	list(APPEND ANDROID_EXTRA_LIBS "${ANDROID_LIBS_ROOT}/libcrypto.so")

	find_package(Qt5AndroidExtras)
else()
	if(CMAKE_INSTALL_LIBDIR)
		set(ANYVNC_LIB_DIR "${CMAKE_INSTALL_LIBDIR}" CACHE INTERNAL "AnyVNC library directory")
	else()
		set(ANYVNC_LIB_DIR lib CACHE INTERNAL "AnyVNC library directory")
	endif()

	set(ANYVNC_INSTALL_PLUGIN_DIR "${ANYVNC_LIB_DIR}/anyvnc")
	set(ANYVNC_INSTALL_DATA_DIR "${CMAKE_INSTALL_DATADIR}/anyvnc")

	if(WIN32)
		set(ANYVNC_PLUGIN_DIR "plugins")
		set(ANYVNC_TRANSLATIONS_DIR "translations")
	else()
		set(ANYVNC_PLUGIN_DIR "../${ANYVNC_LIB_DIR}/anyvnc")
		set(ANYVNC_TRANSLATIONS_DIR "../share/anyvnc/translations")
	endif()
endif()


find_package(Qt5Core)
find_package(Qt5Concurrent)
find_package(Qt5Gui)
find_package(Qt5Widgets)
find_package(Qt5LinguistTools)
find_package(Qt5Quick)
find_package(Qt5QuickControls2)

set(CMAKE_AUTOMOC TRUE)
set(CMAKE_AUTOUIC TRUE)
set(CMAKE_AUTORCC TRUE)

if(SANITIZE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread -fsanitize=undefined")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=thread -fsanitize=undefined")
endif()

set(ANYVNC_COMPILE_OPTIONS "-Wall;-Werror")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fstack-protector-strong ${CFLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fstack-protector-strong -fno-exceptions ${CXXFLAGS}")

add_definitions(
	-D_FORTIFY_SOURCE=2
	-DQT_DEPRECATED_WARNINGS
	-DQT_DISABLE_DEPRECATED_BEFORE=0x050d00
	-DQT_NO_CAST_FROM_ASCII
	-DQT_NO_CAST_TO_ASCII
	-DQT_NO_CAST_FROM_BYTEARRAY
	-DQT_NO_KEYWORDS
	-DQT_NO_NARROWING_CONVERSIONS_IN_CONNECT
	-DQT_USE_QSTRINGBUILDER
	-DQT_STRICT_ITERATORS
	)

if(WIN32 OR WIN64)
	add_definitions(-DUNICODE -D_UNICODE)
	set(DISABLE_LTO TRUE)
endif()

set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${ANYVNC_LIB_DIR}")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

add_subdirectory(libanyvnc)
add_subdirectory(plugins)
add_subdirectory(apps)

#
# add Windows installer related targets
#
if(WIN32)
	include(WindowsInstaller)
endif()

#
# package generation
#
include(cmake/CPackDefinitions.cmake)



#
# display configuration information
#

message("\n"
	"AnyVNC build summary\n"
	"--------------------\n"
	"* Version                     : ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}.${VERSION_BUILD} (${VERSION_STRING})\n"
	"* Install prefix              : ${CMAKE_INSTALL_PREFIX}\n"
	"* Library directory           : ${CMAKE_INSTALL_PREFIX}/${ANYVNC_LIB_DIR}\n"
	"* Plugin directory            : ${CMAKE_INSTALL_PREFIX}/${ANYVNC_INSTALL_PLUGIN_DIR}\n"
	"* Build type                  : ${CMAKE_BUILD_TYPE}\n"
	"* Build platform              : ${CMAKE_SYSTEM_PROCESSOR}\n"
	"* Compile flags               : ${CMAKE_C_FLAGS} (CXX: ${CMAKE_CXX_FLAGS})\n"
	)
