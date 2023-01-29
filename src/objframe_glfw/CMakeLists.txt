file(GLOB SOURCE_FILES *.cpp	)
file(GLOB INCLUDE_FILES *.h) 

set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/appicon.rc")


if(CMAKE_BUILD_TYPE STREQUAL Debug)
	include_directories(BEFORE ${PROJECT_SOURCE_DIR}/src/objframe_glfw ${PROJECT_SOURCE_DIR}/include/ofui )
	add_definitions(-DUSE_FEMVIEW)
	add_compile_options(/bigobj)
	add_executable(objframe_glfwd ${IVF_INCLUDE_DIR} ${INCLUDE_FILES} ${SOURCE_FILES})
else()
	include_directories(BEFORE ${PROJECT_SOURCE_DIR}/src/objframe_glfw)
	add_compile_options(/bigobj)
	add_definitions(-DUSE_FEMVIEW)
	add_executable(objframe_glfw WIN32 ${IVF_INCLUDE_DIR} ${INCLUDE_FILES} ${SOURCE_FILES} ${APP_ICON_RESOURCE_WINDOWS})
	set_target_properties(objframe_glfw PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup /SUBSYSTEM:WINDOWS")
endif()

if(CMAKE_BUILD_TYPE STREQUAL Debug)
    target_link_libraries(objframe_glfwd femd ofsolved visfemd utild ofmathd ivfd ivfmathd ivfimaged ofui_glfwd gleivfd gladd newmatd ${GLFW3_LIBRARY} ${OPENGL_LIBRARY} ${GLU_LIBRARY} ${JPEG_LIBRARY} ${PNG_LIBRARY} ${ZLIB_LIBRARY})
else()
    target_link_libraries(objframe_glfw fem ofsolve visfem util ofmath ivf ivfmath ivfimage ofui_glfw gleivf glad newmat ${GLFW3_LIBRARY} ${OPENGL_LIBRARY} ${GLU_LIBRARY} ${JPEG_LIBRARY} ${PNG_LIBRARY} ${ZLIB_LIBRARY})
endif() 
