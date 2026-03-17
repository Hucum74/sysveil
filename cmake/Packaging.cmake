# cmake/Packaging.cmake
# ── CPack configuration ───────────────────────────────────────────────────────

set(CPACK_PACKAGE_NAME              "SysVeil")
set(CPACK_PACKAGE_VENDOR            "Your Name")
set(CPACK_PACKAGE_DESCRIPTION_SHORT "Real-time cross-platform system resource monitor")
set(CPACK_PACKAGE_DESCRIPTION
    "SysVeil is a lightweight, open-source system resource monitor built with
C++ and Qt Quick. It displays CPU, memory, disk, and network usage in
real-time with animated metric cards and a process manager.")
set(CPACK_PACKAGE_VERSION           "${PROJECT_VERSION}")
set(CPACK_PACKAGE_VERSION_MAJOR     "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR     "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH     "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "SysVeil")
set(CPACK_PACKAGE_EXECUTABLES       "sysveil" "SysVeil")
set(CPACK_RESOURCE_FILE_LICENSE     "${CMAKE_SOURCE_DIR}/LICENSE")

# ── Platform-specific ─────────────────────────────────────────────────────────

if(WIN32)
    set(CPACK_GENERATOR "NSIS")
    set(CPACK_NSIS_DISPLAY_NAME          "SysVeil")
    set(CPACK_NSIS_PACKAGE_NAME          "SysVeil ${PROJECT_VERSION}")
    set(CPACK_NSIS_HELP_LINK             "https://github.com/YOUR_USERNAME/sysveil")
    set(CPACK_NSIS_URL_INFO_ABOUT        "https://github.com/YOUR_USERNAME/sysveil")
    set(CPACK_NSIS_MODIFY_PATH           OFF)
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
    set(CPACK_NSIS_MUI_FINISHPAGE_RUN    "sysveil.exe")
    # Create Start Menu and Desktop shortcuts
    set(CPACK_NSIS_CREATE_ICONS_EXTRA
        "CreateShortcut '$DESKTOP\\\\SysVeil.lnk' '$INSTDIR\\\\bin\\\\sysveil.exe'")
    set(CPACK_NSIS_DELETE_ICONS_EXTRA
        "Delete '$DESKTOP\\\\SysVeil.lnk'")

elseif(APPLE)
    set(CPACK_GENERATOR "DragNDrop")
    set(CPACK_DMG_VOLUME_NAME            "SysVeil ${PROJECT_VERSION}")
    set(CPACK_DMG_FORMAT                 "UDZO")
    set(CPACK_BUNDLE_NAME                "SysVeil")
    set(CPACK_BUNDLE_PLIST               "${CMAKE_SOURCE_DIR}/packaging/macos/Info.plist")

else()
    # Linux — produce a .tar.gz; AppImage is handled separately in CI
    set(CPACK_GENERATOR "TGZ")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER  "Your Name <you@example.com>")
    set(CPACK_DEBIAN_PACKAGE_SECTION     "utils")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS     "libqt6core6, libqt6gui6, libqt6qml6")
endif()

include(CPack)