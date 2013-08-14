file(GLOB SOURCE_FILES *.cpp)
file(GLOB INCLUDE_FILES *.h)

if(CMAKE_BUILD_TYPE STREQUAL Debug)
	add_executable(objframed ${INCLUDE_FILES} ${SOURCE_FILES})
else()
	add_executable(objframe ${INCLUDE_FILES} ${SOURCE_FILES})
endif()

if(CMAKE_BUILD_TYPE STREQUAL Debug)
	target_link_libraries(objframed utild femd visfemd ivfd ivfmathd ivfimaged gleivfd newmatd ${FLTK_BASE_LIBRARY} ${FLTK_GL_LIBRARY} ${JPEG_LIBRARY} ${PNG_LIBRARY} ${ZLIB_LIBRARY} ${OPENGL_LIBRARY} ${GLUT_LIBRARY} ${GLEW_LIBRARY})

else()
	target_link_libraries(objframe util fem visfem ivf ivfmath ivfimage gleivf newmat ${FLTK_BASE_LIBRARY} ${FLTK_GL_LIBRARY} ${JPEG_LIBRARY} ${PNG_LIBRARY} ${ZLIB_LIBRARY} ${OPENGL_LIBRARY} ${GLUT_LIBRARY} ${GLEW_LIBRARY})

endif()
