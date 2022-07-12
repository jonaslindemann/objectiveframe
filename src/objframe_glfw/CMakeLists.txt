file(GLOB SOURCE_FILES *.cpp)
file(GLOB INCLUDE_FILES *.h) 

if(CMAKE_BUILD_TYPE STREQUAL Debug)
	add_executable(objframe_glfwd ${IVF_INCLUDE_DIR} ${INCLUDE_FILES} ${SOURCE_FILES})
else()
	add_executable(objframe_glfw ${IVF_INCLUDE_DIR} ${INCLUDE_FILES} ${SOURCE_FILES})
endif()

if(CMAKE_BUILD_TYPE STREQUAL Debug)
    target_link_libraries(objframe_glfwd ivfd ivfmathd ivfimaged gleivfd gladd ${GLFW3_LIBRARY} ${OPENGL_LIBRARY} )
else()
    target_link_libraries(objframe_glfwd ivf ivfmath ivfimage gleivf glad ${GLFW3_LIBRARY} ${OPENGL_LIBRARY} )
endif()
