set(WINDOWS_INSTALL_FILES "anyvnc-${MINGW_PLATFORM}-${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}.${VERSION_BUILD}")

set(DLLDIR "${MINGW_PREFIX}/bin")
set(DLLDIR_LIB "${MINGW_PREFIX}/lib")
set(DLLDIR_GCC "/usr/lib/gcc/${MINGW_TARGET}/${COMPILER_VERSION_MAJOR_MINOR}-posix")
if(WIN64)
	set(DLL_GCC "libgcc_s_seh-1.dll")
	set(DLL_DDENGINE "ddengine64.dll")
else()
	set(DLL_GCC "libgcc_s_sjlj-1.dll")
	set(DLL_DDENGINE "ddengine.dll")
endif()

add_custom_target(windows-binaries
    COMMAND +env ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --config $<CONFIGURATION>
	COMMAND rm -rf ${WINDOWS_INSTALL_FILES}*
	COMMAND mkdir -p ${WINDOWS_INSTALL_FILES}
	COMMAND cp ${CMAKE_SOURCE_DIR}/plugins/framebuffer/windows/deskdupengine/3rdparty/${DLL_DDENGINE} ${WINDOWS_INSTALL_FILES}
	COMMAND find libanyvnc -name '*.dll' -exec cp '{}' ${WINDOWS_INSTALL_FILES}/ '\;'
	COMMAND find apps -name '*.exe' -exec cp '{}' ${WINDOWS_INSTALL_FILES}/ '\;'
	COMMAND mkdir -p ${WINDOWS_INSTALL_FILES}/plugins
	COMMAND find plugins/ -name '*.dll' -exec cp '{}' ${WINDOWS_INSTALL_FILES}/plugins/ '\;'
	#COMMAND mkdir -p ${WINDOWS_INSTALL_FILES}/translations
	#COMMAND cp translations/*qm ${WINDOWS_INSTALL_FILES}/translations/
	COMMAND cp ${DLLDIR}/libjpeg-62.dll ${WINDOWS_INSTALL_FILES}
	COMMAND cp ${DLLDIR}/libpng16-16.dll ${WINDOWS_INSTALL_FILES}
	COMMAND cp ${DLLDIR}/libcrypto-1_1*.dll ${DLLDIR}/libssl-1_1*.dll ${WINDOWS_INSTALL_FILES}
	COMMAND cp ${DLLDIR}/libvncclient.dll ${WINDOWS_INSTALL_FILES}
	COMMAND cp ${DLLDIR}/libvncserver.dll ${WINDOWS_INSTALL_FILES}
	COMMAND cp ${DLLDIR}/liblzo2-2.dll ${WINDOWS_INSTALL_FILES}
	COMMAND cp ${DLLDIR_LIB}/zlib1.dll ${WINDOWS_INSTALL_FILES}
	COMMAND cp ${DLLDIR_LIB}/libwinpthread-1.dll ${WINDOWS_INSTALL_FILES}
	COMMAND cp ${DLLDIR_GCC}/libstdc++-6.dll ${WINDOWS_INSTALL_FILES}
	COMMAND cp ${DLLDIR_GCC}/libssp-0.dll ${WINDOWS_INSTALL_FILES}
	COMMAND cp ${DLLDIR_GCC}/${DLL_GCC} ${WINDOWS_INSTALL_FILES}
	COMMAND cp ${DLLDIR}/Qt5Core.dll ${DLLDIR}/Qt5Gui.dll ${DLLDIR}/Qt5Network.dll ${DLLDIR}/Qt5Concurrent.dll ${DLLDIR}/Qt5Qml.dll ${DLLDIR}/Qt5Quick.dll ${DLLDIR}/Qt5QuickControls2.dll ${DLLDIR}/Qt5QuickTemplates2.dll ${WINDOWS_INSTALL_FILES}
	COMMAND mkdir -p ${WINDOWS_INSTALL_FILES}/platforms
	COMMAND cp ${DLLDIR_LIB}/qt5/plugins/platforms/qwindows.dll ${WINDOWS_INSTALL_FILES}/platforms
	COMMAND ${STRIP} ${WINDOWS_INSTALL_FILES}/*.dll ${WINDOWS_INSTALL_FILES}/*.exe ${WINDOWS_INSTALL_FILES}/plugins/*.dll ${WINDOWS_INSTALL_FILES}/platforms/*.dll
	COMMAND cp ${CMAKE_SOURCE_DIR}/COPYING ${WINDOWS_INSTALL_FILES}
	COMMAND cp ${CMAKE_SOURCE_DIR}/COPYING ${WINDOWS_INSTALL_FILES}/LICENSE.TXT
	COMMAND cp ${CMAKE_SOURCE_DIR}/README.md ${WINDOWS_INSTALL_FILES}/README.TXT
	COMMAND todos ${WINDOWS_INSTALL_FILES}/*.TXT
	COMMAND cp -ra ${CMAKE_SOURCE_DIR}/nsis ${WINDOWS_INSTALL_FILES}
	COMMAND cp ${CMAKE_BINARY_DIR}/nsis/anyvnc.nsi ${WINDOWS_INSTALL_FILES}
	COMMAND find ${WINDOWS_INSTALL_FILES} -ls
)

add_custom_target(create-windows-installer
	COMMAND makensis ${WINDOWS_INSTALL_FILES}/anyvnc.nsi
	COMMAND mv ${WINDOWS_INSTALL_FILES}/anyvnc-*setup.exe .
	COMMAND rm -rf ${WINDOWS_INSTALL_FILES}
	DEPENDS windows-binaries
)

add_custom_target(prepare-dev-nsi
	COMMAND sed -i ${WINDOWS_INSTALL_FILES}/anyvnc.nsi -e "s,/SOLID lzma,zlib,g"
	DEPENDS windows-binaries)

add_custom_target(dev-nsi
	DEPENDS prepare-dev-nsi
	DEPENDS create-windows-installer
)

