file(GLOB SOURCE_FILES *.cpp)
file(GLOB INCLUDE_FILES *.h)

if(CMAKE_BUILD_TYPE STREQUAL Debug)
	add_executable(objframed ${INCLUDE_FILES} ${SOURCE_FILES})
else()
	add_executable(objframe ${INCLUDE_FILES} ${SOURCE_FILES})
endif()

if(CMAKE_BUILD_TYPE STREQUAL Debug)
	target_link_libraries(objframed utild femd visfemd ivfd ivfmathd ivfimaged imguid imgui_fltkd gleivfd newmatd gladd ws2_32 comctl32 ${FLTK_BASE_LIBRARY} ${GDIPLUS_LIBRARY} ${FLTK_GL_LIBRARY} ${JPEG_LIBRARY} ${PNG_LIBRARY} ${ZLIB_LIBRARY} ${OPENGL_LIBRARY} ${GLUT_LIBRARY} ${GLU_LIBRARY} ${GLEW_LIBRARY} ${FTGL_LIBRARY} ${FREETYPE_LIBRARY})
else()
	target_link_libraries(objframe util fem visfem ivf ivfmath ivfimage imgui imgui_fltk gleivf newmat glad ws2_32 comctl32 ${FLTK_BASE_LIBRARY} ${FLTK_GL_LIBRARY} ${GDIPLUS_LIBRARY} ${JPEG_LIBRARY} ${PNG_LIBRARY} ${ZLIB_LIBRARY} ${OPENGL_LIBRARY} ${GLUT_LIBRARY} ${GLU_LIBRARY} ${GLEW_LIBRARY} ${FTGL_LIBRARY} ${FREETYPE_LIBRARY})
endif()
