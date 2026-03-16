qt_add_executable(sysveil
    core/SystemMonitor.cpp
    qml/MonitorBridge.cpp
    main.cpp
)

# ── QML module ────────────────────────────────────────────────────────────────
qt_add_qml_module(sysveil
    URI "SysVeil"
    VERSION 1.0
    QML_FILES
        qml/main.qml
        qml/theme/Theme.qml
        qml/components/MetricCard.qml
        qml/components/LiveChart.qml
        qml/components/ProcessTable.qml
        qml/components/Sidebar.qml
)

# ── Platform-specific sources ─────────────────────────────────────────────────
if(WIN32)
    target_sources(sysveil PRIVATE
        core/CpuProvider.h
        core/MemoryProvider.h
        core/DiskProvider.h
        core/NetworkProvider.h
        platform/windows/CpuProvider.cpp
        platform/windows/MemoryProvider.cpp
        platform/windows/DiskProvider.cpp
        platform/windows/NetworkProvider.cpp
    )
elseif(APPLE)
    target_sources(sysveil PRIVATE
        core/CpuProvider.h
        core/MemoryProvider.h
        core/DiskProvider.h
        core/NetworkProvider.h
        platform/macos/CpuProvider.cpp
        platform/macos/MemoryProvider.cpp
        platform/macos/DiskProvider.cpp
        platform/macos/NetworkProvider.cpp
    )
else()
    target_sources(sysveil PRIVATE
        core/CpuProvider.h
        core/MemoryProvider.h
        core/DiskProvider.h
        core/NetworkProvider.h
        platform/linux/CpuProvider.cpp
        platform/linux/MemoryProvider.cpp
        platform/linux/DiskProvider.cpp
        platform/linux/NetworkProvider.cpp
    )
endif()

# ── Include directories ───────────────────────────────────────────────────────
target_include_directories(sysveil PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/core
)

# ── Link libraries ────────────────────────────────────────────────────────────
target_link_libraries(sysveil PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Charts
)

# ── Windows: no console window in Release ─────────────────────────────────────
if(WIN32)
    set_target_properties(sysveil PROPERTIES
        WIN32_EXECUTABLE $<CONFIG:Release>
    )
endif()

# ── macOS: bundle metadata ────────────────────────────────────────────────────
if(APPLE)
    set_target_properties(sysveil PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_NAME         "SysVeil"
        MACOSX_BUNDLE_BUNDLE_VERSION      "${PROJECT_VERSION}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
        MACOSX_BUNDLE_GUI_IDENTIFIER      "com.yourname.sysveil"
    )
endif()

# ── Install rules (used by CPack in Step 4) ───────────────────────────────────
include(GNUInstallDirs)
install(TARGETS sysveil
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)