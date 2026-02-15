cmake_minimum_required(VERSION 3.24)

# Auto-detect vcpkg toolchain file, preferring standalone installations over VS bundled vcpkg
# Check if Visual Studio's bundled vcpkg is being used and override with local installations
set(_VS_VCPKG_DETECTED FALSE)
if(DEFINED CMAKE_TOOLCHAIN_FILE)
    string(FIND "${CMAKE_TOOLCHAIN_FILE}" "Microsoft Visual Studio" _VS_VCPKG_POS)
    if(NOT _VS_VCPKG_POS EQUAL -1)
        set(_VS_VCPKG_DETECTED TRUE)
        message(STATUS "Visual Studio bundled vcpkg detected, checking for local installations...")
    endif()
endif()
 
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE OR _VS_VCPKG_DETECTED)
    # Check for VCPKG_ROOT environment variable first
    if(DEFINED ENV{VCPKG_ROOT})
        set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
            CACHE STRING "Vcpkg toolchain file" FORCE)
        message(STATUS "Using vcpkg from VCPKG_ROOT: $ENV{VCPKG_ROOT}")
    # Try common vcpkg installation locations
    elseif(WIN32)
        if(EXISTS "c:/vcpkg/scripts/buildsystems/vcpkg.cmake")
            set(CMAKE_TOOLCHAIN_FILE "c:/vcpkg/scripts/buildsystems/vcpkg.cmake"
                CACHE STRING "Vcpkg toolchain file" FORCE)
            message(STATUS "Found and using vcpkg at: c:/vcpkg")
        elseif(EXISTS "e:/vcpkg/scripts/buildsystems/vcpkg.cmake")
            set(CMAKE_TOOLCHAIN_FILE "e:/vcpkg/scripts/buildsystems/vcpkg.cmake"
                CACHE STRING "Vcpkg toolchain file" FORCE)
            message(STATUS "Found and using vcpkg at: e:/vcpkg")
        elseif(_VS_VCPKG_DETECTED)
            message(STATUS "No local vcpkg found, using Visual Studio bundled vcpkg: ${CMAKE_TOOLCHAIN_FILE}")
        endif()
    elseif(UNIX AND NOT APPLE)
        if(EXISTS "$ENV{HOME}/vcpkg/scripts/buildsystems/vcpkg.cmake")
            set(CMAKE_TOOLCHAIN_FILE "$ENV{HOME}/vcpkg/scripts/buildsystems/vcpkg.cmake"
                CACHE STRING "Vcpkg toolchain file" FORCE)
            message(STATUS "Found and using vcpkg at: $ENV{HOME}/vcpkg")
        elseif(EXISTS "/usr/local/vcpkg/scripts/buildsystems/vcpkg.cmake")
            set(CMAKE_TOOLCHAIN_FILE "/usr/local/vcpkg/scripts/buildsystems/vcpkg.cmake"
                CACHE STRING "Vcpkg toolchain file" FORCE)
            message(STATUS "Found and using vcpkg at: /usr/local/vcpkg")
        endif()
    elseif(APPLE)
        if(EXISTS "$ENV{HOME}/vcpkg/scripts/buildsystems/vcpkg.cmake")
            set(CMAKE_TOOLCHAIN_FILE "$ENV{HOME}/vcpkg/scripts/buildsystems/vcpkg.cmake"
                CACHE STRING "Vcpkg toolchain file" FORCE)
            message(STATUS "Found and using vcpkg at: $ENV{HOME}/vcpkg")
        elseif(EXISTS "/usr/local/vcpkg/scripts/buildsystems/vcpkg.cmake")
            set(CMAKE_TOOLCHAIN_FILE "/usr/local/vcpkg/scripts/buildsystems/vcpkg.cmake"
                CACHE STRING "Vcpkg toolchain file" FORCE)
            message(STATUS "Found and using vcpkg at: /usr/local/vcpkg")
        endif()
    endif()

    if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
        message(WARNING "vcpkg toolchain file not found. Please set VCPKG_ROOT environment variable or CMAKE_TOOLCHAIN_FILE.")
    endif()
endif()

# Share vcpkg installed packages across all build directories to save disk space
# This puts vcpkg_installed/ in the project root instead of each build directory
set(VCPKG_INSTALLED_DIR "${CMAKE_SOURCE_DIR}/vcpkg_installed" CACHE PATH "vcpkg installed directory")

project(objframe2)

enable_language(CXX)
enable_language(C)
set(CMAKE_CXX_STANDARD 20)

# Enable automatic debug postfix for all targets
set(CMAKE_DEBUG_POSTFIX d)

# Set output directories with proper Debug/Release structure on Windows
if(WIN32)
  # On Windows, separate Debug and Release libraries into subdirectories
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/lib/Debug)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/lib/Debug)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/bin/Debug)
  
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/lib/Release)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/lib/Release)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/bin/Release)
  
  # Also set for other configurations
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO ${PROJECT_SOURCE_DIR}/lib/RelWithDebInfo)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO ${PROJECT_SOURCE_DIR}/lib/RelWithDebInfo)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${PROJECT_SOURCE_DIR}/bin/RelWithDebInfo)
  
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL ${PROJECT_SOURCE_DIR}/lib/MinSizeRel)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL ${PROJECT_SOURCE_DIR}/lib/MinSizeRel)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${PROJECT_SOURCE_DIR}/bin/MinSizeRel)
else()
  # On Unix/Linux, use simple directory structure
  set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
  set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
endif()

IF(MSVC)
  SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)
ENDIF()

find_package(PNG REQUIRED)
find_package(JPEG REQUIRED)
find_package(ZLIB REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(CURL CONFIG REQUIRED) 

find_path(CHAISCRIPT_INCLUDE_DIRS "chaiscript/chaiscript.hpp")

option(USE_LEAP "Build with support for LeapMotion" OFF)

set(IVF_ROOT "${PROJECT_SOURCE_DIR}/../ivfplusplus")
set(IVF_INCLUDE_DIR ${IVF_ROOT}/include/vc ${IVF_ROOT}/include)

# Create imported targets for IVF++ libraries with proper debug postfix handling
# This works with both single-config and multi-config generators
set(IVF_LIBRARIES ivf ivfgle ivfmath ivfimage gleivf glad)

foreach(lib ${IVF_LIBRARIES})
  add_library(${lib} STATIC IMPORTED)
  set_target_properties(${lib} PROPERTIES
    IMPORTED_LOCATION_RELEASE "${IVF_ROOT}/lib/Release/${lib}.lib"
    IMPORTED_LOCATION_DEBUG "${IVF_ROOT}/lib/Debug/${lib}d.lib"
    INTERFACE_INCLUDE_DIRECTORIES "${IVF_INCLUDE_DIR}"
  )
  # For single-config generators on Unix/Linux (if needed)
  if(NOT WIN32)
    set_target_properties(${lib} PROPERTIES
      IMPORTED_LOCATION "${IVF_ROOT}/lib/${lib}.a"
    )
  endif()
endforeach()

if (APPLE OR UNIX)
  # Use generator expression to define _DEBUG for debug configurations
  add_compile_definitions($<$<CONFIG:Debug>:_DEBUG>)
endif()

if (LINUX)
  set(MISC_LIBRARIES
      "X11 Xft Xcursor Xinerama Xrender Xfixes fontconfig pthread")
endif()

set(CHAISCRIPT_EXTRAS_INCLUDE ${PROJECT_SOURCE_DIR}/include)

if(USE_LEAP)
  set(LEAP_LIBS ${PROJECT_SOURCE_DIR}/LeapSDK/lib)
  set(LEAP_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/LeapSDK/include)
else()
  set(LEAP_LIBS "")
  set(LEAP_INCLUDE_DIR "")
endif()

configure_file("${PROJECT_SOURCE_DIR}/include/ObjframeConfig.h.in"
               "${PROJECT_SOURCE_DIR}/include/ObjframeConfig.h")

set(OBJFRAME_INCLUDE_DIR
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/include/vc
    ${PROJECT_SOURCE_DIR}/src/fem
    ${PROJECT_SOURCE_DIR}/src/visfem
    ${PROJECT_SOURCE_DIR}/src/newmat
    ${PROJECT_SOURCE_DIR}/src/util
    ${PROJECT_SOURCE_DIR}/src/imgui 
    ${PROJECT_SOURCE_DIR}/src/imgui_fltk
    ${PROJECT_SOURCE_DIR}/src/objframe)

include_directories(
  ${DEPENDS_INCLUDE_DIR} ${OBJFRAME_INCLUDE_DIR} ${IVF_INCLUDE_DIR}
  ${OBJFRAME_INCLUDE} ${CHAISCRIPT_EXTRAS_INCLUDE})

# Link directories for ObjectiveFrame libraries (with Debug/Release subfolders on Windows)
if(WIN32)
  # On Windows, libraries are in lib/Debug and lib/Release with 'd' suffix for debug
  link_directories(
    ${PROJECT_SOURCE_DIR}/lib/Debug
    ${PROJECT_SOURCE_DIR}/lib/Release
    ${LEAP_LIBS}
  )
else()
  # On Unix/Linux, typically in a single lib directory
  link_directories(${PROJECT_SOURCE_DIR}/lib ${DEPENDS_LIB_DIR} ${LEAP_LIBS})
endif()

if(WIN32)
  if(IVF_DEBUG)
    string(REPLACE "/Zi" "/Z7" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
    string(REPLACE "/Zi" "/Z7" CMAKE_C_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG})
  endif()
endif()


add_subdirectory(src)

# Create unified objframe interface library target
# This bundles all objframe libraries and dependencies for easier linking
add_library(objframe INTERFACE)
add_library(objframe::objframe ALIAS objframe)

target_link_libraries(objframe INTERFACE
    ofui_glfw
    ofservice
    ofai
    ofsolve
    visfem
    util
    ofmath
    fem
    newmat
    imguifd
    civetweb
    glad
    PNG::PNG
    JPEG::JPEG
    ZLIB::ZLIB
    glfw
    GLEW::GLEW
    OpenGL::GL
    CURL::libcurl
)

target_include_directories(objframe INTERFACE
    ${PROJECT_SOURCE_DIR}/include
    ${OBJFRAME_INCLUDE_DIR}
    ${IVF_INCLUDE_DIR}
    ${CHAISCRIPT_EXTRAS_INCLUDE}
)
