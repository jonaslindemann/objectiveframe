cmake_minimum_required(VERSION 3.23)
# set(CMAKE_TOOLCHAIN_FILE
# E:/Users/Jonas/Development/vcpkg/scripts/buildsystems/vcpkg.cmake)

project(objframe2)
 
set(CMAKE_CXX_STANDARD 17)

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

find_package(PNG REQUIRED) 
find_package(JPEG REQUIRED)
find_package(ZLIB REQUIRED)
find_package(FLTK CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)

find_path(CHAISCRIPT_INCLUDE_DIRS "chaiscript/chaiscript.hpp")

option(USE_LEAP "Build with support for LeapMotion" OFF)

set(IVF_ROOT "${PROJECT_SOURCE_DIR}/../ivfplusplus")

set(IVF_INCLUDE_DIR ${IVF_ROOT}/include/vc ${IVF_ROOT}/include)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(IVF_LIBS ${IVF_ROOT}/lib/Debug)
else()
  set(IVF_LIBS ${IVF_ROOT}/lib/Release)
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

if(WIN32)
  link_directories(${PROJECT_SOURCE_DIR}/lib ${IVF_LIBS} ${LEAP_LIBS})
else()
  link_directories(${PROJECT_SOURCE_DIR}/lib ${DEPENDS_LIB_DIR} ${IVF_LIBS}
                   ${LEAP_LIBS})
endif()

if(WIN32)
  if(IVF_DEBUG)
    string(REPLACE "/zi" "/z7" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
    string(REPLACE "/zi" "/z7" CMAKE_C_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG})
  endif(IVF_DEBUG)
endif(WIN32)

subdirs(src)
