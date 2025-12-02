file(GLOB SOURCE_FILES *.cpp)
file(GLOB INCLUDE_FILES *.h)

add_executable(objframe ${INCLUDE_FILES} ${SOURCE_FILES})
set_target_properties(objframe PROPERTIES DEBUG_POSTFIX d)

if (WIN32)
	target_link_libraries(objframe util fem visfem ivf ivfmath ivfimage ofui imgui_fltk gleivf newmat glad ws2_32 comctl32 ${FLTK_BASE_LIBRARY} ${FLTK_GL_LIBRARY} ${GDIPLUS_LIBRARY} ${JPEG_LIBRARY} ${PNG_LIBRARY} ${ZLIB_LIBRARY} ${OPENGL_LIBRARY} ${GLUT_LIBRARY} ${GLU_LIBRARY} ${GLEW_LIBRARY} ${FTGL_LIBRARY} ${FREETYPE_LIBRARY})
else()
	target_link_libraries(objframe fem visfem util ivf ivfmath ivfimage ofui imgui_fltk gleivf newmat glad ${FLTK_BASE_LIBRARY} ${FLTK_GL_LIBRARY} dl GLU GL X11 Xft Xcursor Xinerama Xrender Xfixes fontconfig pthread ${GDIPLUS_LIBRARY} ${JPEG_LIBRARY} ${PNG_LIBRARY} ${ZLIB_LIBRARY} ${OPENGL_LIBRARY} ${GLUT_LIBRARY} ${GLU_LIBRARY} ${GLEW_LIBRARY} ${FTGL_LIBRARY} ${FREETYPE_LIBRARY})	
endif() 
