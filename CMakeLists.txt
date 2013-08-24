project(objframe2)

cmake_minimum_required(VERSION 2.6)

set( LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib )
set( EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin )

set( IVF_ROOT /Users/jonas/Development/ivf )
set( IVF_INCLUDE_DIR ${IVF_ROOT}/include/vc ${IVF_ROOT}/include )
set( IVF_LIBS ${IVF_ROOT}/lib )

find_package(OpenGL)
find_package(FLTK)
find_package(GLEW)
find_package(JPEG)
find_package(PNG)
find_package(ZLIB)

set( FTGL_INCLUDE_DIR ${FTGL_ROOT}/src )
set( FTGL_LIBS ${FTGL_ROOT}/msvc/Build )

set( OBJFRAME_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include/vc ${PROJECT_SOURCE_DIR}/src/fem ${PROJECT_SOURCE_DIR}/src/visfem ${PROJECT_SOURCE_DIR}/src/newmat ${PROJECT_SOURCE_DIR}/src/util)

include_directories( ${OBJFRAME_INCLUDE_DIR} ${FLTK_INCLUDE_DIR} ${JPEG_INCLUDE_DIR} ${PNG_PNG_INCLUDE_DIR} ${ZLIB_INCLUDE_DIR} ${GLEW_INCLUDE_DIR} ${IVF_INCLUDE_DIR} ${OBJFRAME_INCLUDE} )

link_directories( ${PROJECT_SOURCE_DIR}/lib ${IVF_LIBS} )

if(WIN32)
    if(IVF_DEBUG)
		string(REPLACE "/zi" "/z7" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
		string(REPLACE "/zi" "/z7" CMAKE_C_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG})
    endif(IVF_DEBUG)
endif(WIN32)

subdirs(src)
