# CMake Build script for Mudbox Deformers plugin.
#
# NOTE: (sonictk) Requires the ``MUDBOX_SDK_ROOT`` to be set either in the environment
# or passed to CMake.
# You can set ``MUDBOX_PLUGINS_DEPLOYMENT_PATH`` if you would like to install the plugins
# directly to a different location as well (Can be set to your Mudbox plugins directory
# i.e. ``C:\Program Files\Autodesk\Mudbox 2018\plugins`` if you would like to be
# able to use your plugins immediately after building.)

cmake_minimum_required(VERSION 3.0.2)
project(mudbox_deformers)

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
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(LINUX TRUE)
    set(MUDBOX_PLUGIN_EXTENSION ".so")
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(MACOSX TRUE)
    set(MUDBOX_PLUGIN_EXTENSION ".dylib")
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(
    "${PROJECT_SOURCE_DIR}/thirdparty"
    "${MUDBOX_SDK_ROOT}/include")

# TODO: (sonictk) Setup for clang and gcc
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(STATUS "Detected compiler as Clang...")
    # Using Clang
    set(CMAKE_CXX_FLAGS "-std=c++0x -pthread")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g -gdwarf-4 -gmodules -O0")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # using GCC on Linux
    set(CMAKE_CXX_FLAGS "-std=c++0x -pthread -fno-rtti -Winline")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -ggdb -O0")
elseif(MSVC)
    # using Visual Studio on Windows
    set(CMAKE_CXX_FLAGS "/GS /W3 /Zc:wchar_t /Zc:inline /fp:precise /errorReport:none /WX- /EHsc /nologo /MP /arch:AVX2 /std:c++14")
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

    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /nologo /subsystem:windows /manifest /dynamicbase:no /dll /machine:X64 /manifestuac:"level=asInvoker uiAccess=false" /errorReport:none /tlbid:1")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS} /debug")

    # NOTE: (sonictk) Turn on link optimizations for eliminating unreferenced
    # functions/data and identical COMDAT folding
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS} /opt:REF /opt:ICF")
endif()

# Unity build
set(PLUGIN_ENTRY_POINT mudbox_deformers_main.cpp)

# This allows CMake to expose sources to IDEs when generating projects for them (e.g. VS or XCode)
set(PLUGIN_SOURCES
    mudbox_deformers_spherify_deformer.h
    mudbox_deformers_spherify_deformer.cpp
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
