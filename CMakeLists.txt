project(objframe2)

cmake_minimum_required(VERSION 3.0)

set( LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib )
set( EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin )

option( USE_LEAP "Build with support for LeapMotion" OFF)

set( IVF_ROOT "d:/Users/Jonas/Development/ivfplusplus" )
set( IVF_INCLUDE_DIR ${IVF_ROOT}/include/vc ${IVF_ROOT}/include )
set( IVF_LIBS ${IVF_ROOT}/lib )

if (WIN32)
        if (CMAKE_BUILD_TYPE STREQUAL "Debug")
                message("Debug libraries...")
                set(FLTK_INCLUDE_DIR "d:/Libraries/fltk-1.4")
                set(FLTK_BASE_LIBRARY "d:/Libraries/fltk-1.4/build_debug/lib/Debug/fltkd.lib")
                set(FLTK_GL_LIBRARY "d:/Libraries/fltk-1.4/build_debug/lib/Debug/fltk_gld.lib")
                set(JPEG_INCLUDE_DIR "d:/Libraries/fltk-1.4/jpeg")
                set(JPEG_LIBRARY "d:/Libraries/fltk-1.4/build_debug/lib/Debug/fltk_jpegd.lib")
                set(PNG_INCLUDE_DIR "d:/Libraries/fltk-1.4/png")
                set(PNG_LIBRARY "d:/Libraries/fltk-1.4/build_debug/lib/Debug/fltk_pngd.lib")
                set(ZLIB_INCLUDE_DIR "d:/Libraries/fltk-1.4/zlib")
                set(ZLIB_LIBRARY "d:/Libraries/fltk-1.4/build_debug/lib/Debug/fltk_zd.lib")
                set(GLEW_INCLUDE_DIR "d:/Libraries/glew/include")
                set(GLEW_LIBRARY "d:/Libraries/glew/lib/Debug/Win32/glew32sd.lib")
                #set(FREETYPE_INCLUDE_DIR
                set(OPENGL_LIBRARY "opengl32" "glu32")
                set(GLUT_INCLUDE_DIR "D:/Libraries/freeglut/include")
                set(GLUT_LIBRARY "D:/Libraries/freeglut/build-debug/lib/Debug/freeglut_staticd.lib")
        else()
                set(FLTK_DIR "E:/Libraries/fltk-1.4/build_release")
                set(FLTK_INCLUDE_DIR "E:/Libraries/fltk-1.4")
                set(GLEW_INCLUDE_DIR "E:/Libraries/glew-2.1.0/include")
                set(GLEW_LIBRARY "E:/Libraries/glew-2.1.0/lib/Release/Win32/glew32.lib")
        endif()
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

#find_package(OpenGL)
#find_package(FLTK)
#find_package(GLEW)
#find_package(JPEG)
#find_package(PNG)
#find_package(ZLIB)


set( FTGL_INCLUDE_DIR ${FTGL_ROOT}/src )
set( FTGL_LIBS ${FTGL_ROOT}/msvc/Build )

set( OBJFRAME_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/include/vc ${PROJECT_SOURCE_DIR}/src/fem ${PROJECT_SOURCE_DIR}/src/visfem ${PROJECT_SOURCE_DIR}/src/newmat ${PROJECT_SOURCE_DIR}/src/util)

include_directories( ${OBJFRAME_INCLUDE_DIR} ${FLTK_INCLUDE_DIR} ${JPEG_INCLUDE_DIR} ${PNG_PNG_INCLUDE_DIR} ${ZLIB_INCLUDE_DIR} ${GLEW_INCLUDE_DIR} ${IVF_INCLUDE_DIR} ${OBJFRAME_INCLUDE} ${LEAP_INCLUDE_DIR} )

link_directories( ${PROJECT_SOURCE_DIR}/lib ${IVF_LIBS} ${LEAP_LIBS} )

if(WIN32)
    if(IVF_DEBUG)
		string(REPLACE "/zi" "/z7" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
		string(REPLACE "/zi" "/z7" CMAKE_C_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG})
    endif(IVF_DEBUG)
endif(WIN32)

subdirs(src)
