# CMake Build script for Mudbox Deformers plugin.
#
# NOTE: (sonictk) Requires the ``MUDBOX_SDK_ROOT`` to be set either in the environment
# or passed to CMake.
# You can set ``MUDBOX_PLUGINS_DEPLOYMENT_PATH`` if you would like to install the plugins
# directly to a different location as well (Can be set to your Mudbox plugins directory
# i.e. ``C:\Program Files\Autodesk\Mudbox 2018\plugins`` if you would like to be
# able to use your plugins immediately after building.)

# Official Autodesk recommended compiler versions for Mudbox 2018
# Windows is Windows 7 and Visual Studio 12
# (But it looks like Visual Studio 11 is what most of the existing plugins are compiled against)
# Mac OS is OSX 10.9.2 Xcode 6.1
# Linux 2.6.32 GCC 4.4.6-3

# Target for Mudbox 2019 PR and beyond
# Windows 10 and Visual Studio 2015
# Mac OSX 10.13.x  Xcode 9.x
# Linux Centos 7.0 and  gcc 6.3.1 / glib 2.17
# Qt 5 being targeted for EOY 2018

cmake_minimum_required(VERSION 3.6.3)
project(mudbox_deformers)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/modules)

if(MUDBOX_SDK_ROOT)
    message(STATUS "Using Mudbox SDK at: ${MUDBOX_SDK_ROOT}")
elseif(ENV{MUDBOX_SDK_ROOT})
    message(STATUS "Using Mudbox SDK at: $ENV{MUDBOX_SDK_ROOT}")
    set(MUDBOX_SDK_ROOT $ENV{MUDBOX_SDK_ROOT})
else()
    message(FATAL_ERROR "Cannot find Mudbox SDK! Specify MUDBOX_SDK_ROOT to point to the directory!")
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(WINDOWS TRUE)
    set(MUDBOX_PLUGIN_EXTENSION ".mp")
    set(VS_STARTUP_PROJECT "${PROJECT_NAME}")
    set(QT_ROOT_DIR "C:\\Qt\\4.8.6")
    set(QT_QMAKE_EXECUTABLE "${QT_ROOT_DIR}\\bin\\qmake.exe")
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(LINUX TRUE)
    set(MUDBOX_PLUGIN_EXTENSION ".so")
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(MACOSX TRUE)
    set(MUDBOX_PLUGIN_EXTENSION ".dylib")
endif()

# NOTE: (sonictk) Relying on system-installed Qt here. Eventually need to wrap
# the Mudbox ones since I have multiple versions of Qt installed.
set(CMAKE_AUTOMOC ON)
find_package(Qt4 4.8.6 REQUIRED QtGui QtCore)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(
    "${MUDBOX_SDK_ROOT}/include")

# TODO: (sonictk) Setup for Linux/Mac
if(MACOSX)
    message(STATUS "Detected compiler as Clang...")
    # Using Clang
    set(CMAKE_CXX_FLAGS "-std=c++0x -pthread")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g -gdwarf-4 -gmodules -O0")

elseif(LINUX)
    # using GCC on Linux
    set(CMAKE_CXX_FLAGS "-std=c++0x -pthread -fno-rtti -Winline")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -ggdb -O0")

elseif(WINDOWS)
    # using Visual Studio on Windows
    set(CMAKE_CXX_FLAGS "/GS /W3 /Zc:wchar_t /fp:precise /errorReport:none /WX- /EHsc /nologo /MP /wd4996")
    set(CMAKE_CXX_FLAGS_RELEASE "/Ox /Oi /GL /MD")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} /Zi /Od /MDd")

    add_definitions(-DWIN32
                    -DNDEBUG
                    -DWINDOWS
                    -D_USRDLL
                    -DCAMERASHOTS_EXPORTS
                    -DMB_USE_QT
                    -D_WINDLL
                    -D_UNICODE
                    -DUNICODE)

    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /nologo /subsystem:windows /manifest /dynamicbase:no /dll /machine:X64 /errorReport:none /tlbid:1")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS} /debug")

    # NOTE: (sonictk) Turn on link optimizations for eliminating unreferenced
    # functions/data and identical COMDAT folding
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS} /opt:REF /opt:ICF")

endif()

# Unity build
set(PLUGIN_ENTRY_POINT mudbox_deformers_main.cpp)

# This allows CMake to expose sources to IDEs when generating projects for them (e.g. VS or XCode)
set(PLUGIN_SOURCES
    mudbox_deformers_common_math.h
    mudbox_deformers_util.h
    mudbox_deformers_util.cpp
    mudbox_deformers_spherify_deformer.h
    mudbox_deformers_spherify_deformer.cpp
    mudbox_deformers_bend_deformer.h
    mudbox_deformers_bend_deformer.cpp
    mudbox_deformers_noise_deformer.h
    mudbox_deformers_noise_deformer.cpp
    mudbox_deformers_noise.h
    mudbox_deformers_noise.cpp
    mudbox_deformers_main.h)

set_source_files_properties(${PLUGIN_SOURCES} PROPERTIES HEADER_FILE_ONLY TRUE)

set(MUDBOX_LIBRARIES
    "${MUDBOX_SDK_ROOT}/lib/MudboxFramework.lib"
    "${MUDBOX_SDK_ROOT}/lib/QtGui4.lib"
    "${MUDBOX_SDK_ROOT}/lib/QtCore4.lib")

if(WINDOWS)
    set(OS_LIBRARIES
        "Shlwapi.lib"
        "advapi32.lib"
        "comdlg32.lib"
        "gdi32.lib"
        "kernel32.lib"
        "odbc32.lib"
        "odbccp32.lib"
        "ole32.lib"
        "oleaut32.lib"
        "opengl32.lib"
        "shell32.lib"
        "user32.lib"
        "uuid.lib"
        "winspool.lib")
else()
    set(OS_LIBRARIES
        GL GLEW glfw3 dl X11 Xrandr Xi pthread Xinerama Xcursor Xxf86vm)
endif()

# find_package(Qt4Macros REQUIRED)
# qt_wrap_cpp(${PROJECT_NAME} MOC_SOURCES "${PROJECT_SOURCE_DIR}/mudbox_deformers_bend_deformer.h")
# add_library(${PROJECT_NAME} SHARED ${MOC_SOURCES} ${PLUGIN_SOURCES} ${PLUGIN_ENTRY_POINT})

add_library(${PROJECT_NAME} SHARED ${PLUGIN_SOURCES} ${PLUGIN_ENTRY_POINT})

target_link_libraries(${PROJECT_NAME} ${MUDBOX_LIBRARIES} ${OS_LIBRARIES})

set_target_properties(${PROJECT_NAME} PROPERTIES
    SUFFIX ${MUDBOX_PLUGIN_EXTENSION})

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/bin)
install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX})

# NOTE: (sonictk) Install plugins directly for use immediately
if(MUDBOX_PLUGINS_DEPLOYMENT_PATH)
    message(STATUS "Installing plugins to deployment location: ${MUDBOX_PLUGINS_DEPLOYMENT_PATH}")
    install(TARGETS ${PROJECT_NAME} DESTINATION ${MUDBOX_PLUGINS_DEPLOYMENT_PATH})
endif()
