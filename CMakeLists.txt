cmake_minimum_required(VERSION 3.21)

project(SysVeil
    VERSION 0.1.0
    DESCRIPTION "Real-time cross-platform system resource monitor"
    HOMEPAGE_URL "https://github.com/hucum74/sysveil"
    LANGUAGES CXX
)

# -- C++ Standard --
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# -- Qt configuration --
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.2 REQUIRED COMPONENTS
    Core
    Gui
    Quick
    QuickControls2
    Charts
)

qt_standard_project_setup(REQUIRES 6.2)

# -- Platform detection --
if(WIN32)
    set(SYSVEIL_PLATFORM "windows")
elseif(APPLE)
    set(SYSVEIL_PLATFORM "macos")
elseif(UNIX AND NOT APPLE)
    set(SYSVEIL_PLATFORM "linux")
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

message(STATUS "SysVeil Platform: ${SYSVEIL_PLATFORM}")
message(STATUS "SysVeil Version: ${PROJECT_VERSION}")

# -- Build type default
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo")
endif()

# -- Subdirectories
add_subdirectory(src)

# -- Packaging (stub )
include(cmake/Packaging.cmake)