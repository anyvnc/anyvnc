#
# generate packages
#
# Environment
if(NOT CPACK_SYSTEM_NAME)
	set(CPACK_SYSTEM_NAME "${CMAKE_SYSTEM_PROCESSOR}")
endif()


# Basic information
set(CPACK_PACKAGE_NAME "anyvnc")
set(CPACK_PACKAGE_VERSION_MAJOR "${VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")

set(CPACK_PACKAGING_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}_${CPACK_PACKAGE_VERSION}_${CPACK_SYSTEM_NAME}")
set(CPACK_PACKAGE_CONTACT "Tobias Junghans <tobydox@veyon.io>")
set(CPACK_PACKAGE_HOMEPAGE "https://anyvnc.com")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "")
set(CPACK_PACKAGE_VENDOR "Veyon Solutions")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY TRUE)
set(CPACK_SOURCE_IGNORE_FILES "${CMAKE_SOURCE_DIR}/build/;${CMAKE_SOURCE_DIR}/.git/;")
set(CPACK_STRIP_FILES  TRUE)

# DEB package
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "")
set(CPACK_DEBIAN_PACKAGE_SECTION "Education")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "qml-module-qtquick2, qml-module-qtquick-dialogs, qml-module-qtquick-layouts, qml-module-qtqml-models2, qml-module-qtquick-controls2, qml-module-qtquick-window2")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_COMPRESSION_TYPE "xz")

function(ReadRelease valuename FROM filename INTO varname)
  file (STRINGS ${filename} _distrib
	REGEX "^${valuename}="
	)
  string (REGEX REPLACE
	"^${valuename}=\"?\(.*\)" "\\1" ${varname} "${_distrib}"
	)
  # remove trailing quote that got globbed by the wildcard (greedy match)
  string (REGEX REPLACE
	"\"$" "" ${varname} "${${varname}}"
	)
  set (${varname} "${${varname}}" PARENT_SCOPE)
ENDfunction()

# RPM package
if(EXISTS /etc/os-release)
ReadRelease("NAME" FROM /etc/os-release INTO OS_NAME)
if(OS_NAME MATCHES ".*openSUSE.*")
	set(OS_OPENSUSE TRUE)
endif()
endif()

if(OS_OPENSUSE)
set(CPACK_RPM_PACKAGE_REQUIRES ${CPACK_RPM_PACKAGE_REQUIRES} "libqt5-qtquickcontrols2")
else()
set(CPACK_RPM_PACKAGE_REQUIRES ${CPACK_RPM_PACKAGE_REQUIRES} "qt5-qtquickcontrols2")
endif()
set(CPACK_RPM_PACKAGE_LICENSE "GPLv2")
set(CPACK_RPM_PACKAGE_DESCRIPTION ${CPACK_DEBIAN_PACKAGE_DESCRIPTION})
set(CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION /lib)


if(UNIX)
  if(EXISTS /etc/redhat-release OR EXISTS /etc/fedora-release OR OS_OPENSUSE)
	set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}.${CPACK_SYSTEM_NAME}")
	set(CPACK_GENERATOR "RPM")
  endif()
  if(EXISTS /etc/debian_version)
	if(CPACK_SYSTEM_NAME STREQUAL "x86_64")
			set(CPACK_SYSTEM_NAME "amd64")
	endif()
	set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}_${CPACK_PACKAGE_VERSION}_${CPACK_SYSTEM_NAME}")
	set(CPACK_GENERATOR "DEB")
  endif()
  set(CPACK_SOURCE_GENERATOR "TGZ")
endif()

include(CPack)
