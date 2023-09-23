cmake_minimum_required(VERSION 3.23)
#set(CMAKE_TOOLCHAIN_FILE E:/Users/Jonas/Development/vcpkg/scripts/buildsystems/vcpkg.cmake)

project(objframe2)

set (CMAKE_CXX_STANDARD 17)

set( LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib )
set( EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin )

find_package(PNG REQUIRED)
#target_link_libraries(main PRIVATE PNG::PNG)
find_package(JPEG REQUIRED)
#target_include_directories(main PRIVATE JPEG::JPEG)
find_package(ZLIB REQUIRED)
#target_link_libraries(main PRIVATE ZLIB::ZLIB)
find_package(FLTK CONFIG REQUIRED)
#target_link_libraries(main PRIVATE fltk fltk_gl fltk_forms fltk_images)
find_package(glfw3 CONFIG REQUIRED)
#target_link_libraries(main PRIVATE glfw)
find_package(GLEW REQUIRED)
#target_link_libraries(main PRIVATE GLEW::GLEW)
find_package(OpenGL REQUIRED)
#target_link_libraries(main PRIVATE OpenGL::GL)

find_path(CHAISCRIPT_INCLUDE_DIRS "chaiscript/chaiscript.hpp")
#target_include_directories(main PRIVATE ${CHAISCRIPT_INCLUDE_DIRS})


option( USE_LEAP "Build with support for LeapMotion" OFF)

if (WIN32)
    set( LIB_DRIVE "d:")
    set( IVF_ROOT "${LIB_DRIVE}/Libraries/ivfplusplus" )
else()
    set( IVF_ROOT "${PROJECT_SOURCE_DIR}/../ivfplusplus" )
endif()

message(${IVF_ROOT})

set( IVF_INCLUDE_DIR ${IVF_ROOT}/include/vc ${IVF_ROOT}/include )

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set( IVF_LIBS ${IVF_ROOT}/lib/Debug )
else()
    set( IVF_LIBS ${IVF_ROOT}/lib/Release )
endif()

if (WIN32)
        if (CMAKE_BUILD_TYPE STREQUAL "Debug")
                message("Debug libraries...")
                set(CHAISCRIPT_EXTRAS_INCLUDE "${LIB_DRIVE}/Libraries/ChaiScript_Extras/include")
        else()
                message("Release libraries...")
                set(CHAISCRIPT_EXTRAS_INCLUDE "${LIB_DRIVE}/Libraries/ChaiScript_Extras/include")
        endif()
else()
    message("Release libraries...")
    set(FLTK_BASE_LIBRARY "fltk")
    set(FLTK_GL_LIBRARY "fltk_gl")
    set(JPEG_LIBRARY "jpeg")
    set(PNG_LIBRARY "png16")
    set(ZLIB_LIBRARY "z")
    set(OPENGL_LIBRARY "GL")
    set(GLU_LIBRARY "GLU")
    set(MISC_LIBRARIES "X11 Xft Xcursor Xinerama Xrender Xfixes fontconfig pthread")
    set(CHAISCRIPT_EXTRAS_INCLUDE "E:/Users/Jonas/Development/ChaiScript_Extras/include")
    set(GLFW3_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/../depends/include")
    set(GLFW3_LIBRARY "${PROJECT_SOURCE_DIR}/../depends/lib64/libglfw3.a")
    set(DEPENDS_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/../depends/include")
    set(DEPENDS_LIB_DIR "${PROJECT_SOURCE_DIR}/../depends/lib64")
endif()

if (USE_LEAP)
    set( LEAP_LIBS ${PROJECT_SOURCE_DIR}/LeapSDK/lib )
    set( LEAP_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/LeapSDK/include )
else()
    set( LEAP_LIBS "" )
    set( LEAP_INCLUDE_DIR "" )
endif() 

configure_file (
    "${PROJECT_SOURCE_DIR}/include/ObjframeConfig.h.in"
    "${PROJECT_SOURCE_DIR}/include/ObjframeConfig.h"
)

set( OBJFRAME_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/include/vc ${PROJECT_SOURCE_DIR}/src/fem ${PROJECT_SOURCE_DIR}/src/visfem ${PROJECT_SOURCE_DIR}/src/newmat ${PROJECT_SOURCE_DIR}/src/util ${PROJECT_SOURCE_DIR}/src/imgui ${PROJECT_SOURCE_DIR}/src/imgui_fltk ${PROJECT_SOURCE_DIR}/src/objframe)

include_directories( ${DEPENDS_INCLUDE_DIR} ${OBJFRAME_INCLUDE_DIR} ${IVF_INCLUDE_DIR} ${OBJFRAME_INCLUDE} ${CHAISCRIPT_EXTRAS_INCLUDE} )

message( ${IVF_INCLUDE_DIR} ) 

if(win32)
    link_directories( ${PROJECT_SOURCE_DIR}/lib ${IVF_LIBS} ${LEAP_LIBS})
else()
    link_directories( ${PROJECT_SOURCE_DIR}/lib ${DEPENDS_LIB_DIR} ${IVF_LIBS} ${LEAP_LIBS})
endif()

if(WIN32)
    if(IVF_DEBUG)
        string(REPLACE "/zi" "/z7" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
        string(REPLACE "/zi" "/z7" CMAKE_C_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG})
    endif(IVF_DEBUG)
endif(WIN32)

subdirs(src)
