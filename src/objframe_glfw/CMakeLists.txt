file(GLOB SOURCE_FILES *.cpp)
file(GLOB INCLUDE_FILES *.h) 

set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/appicon.rc")

include_directories(BEFORE ${PROJECT_SOURCE_DIR}/src/objframe_glfw ${PROJECT_SOURCE_DIR}/include/ofui)
add_definitions(-DUSE_FEMVIEW)

if (WIN32)
	add_compile_options(/bigobj)
endif()

# Create a single executable that works for both Debug and Release
# Use WIN32 flag only for Release builds using generator expressions
add_executable(objframe_glfw ${IVF_INCLUDE_DIR} ${INCLUDE_FILES} ${SOURCE_FILES} ${APP_ICON_RESOURCE_WINDOWS})
target_include_directories(objframe_glfw PRIVATE ${CHAISCRIPT_INCLUDE_DIRS})

if (WIN32)
	# Set WIN32 subsystem only for Release builds
	set_target_properties(objframe_glfw PROPERTIES 
		WIN32_EXECUTABLE $<CONFIG:Release>
		LINK_FLAGS_RELEASE "/ENTRY:mainCRTStartup /SUBSYSTEM:WINDOWS"
	)
endif()

# Link libraries with automatic debug postfix handling
if (WIN32)
	target_link_libraries(objframe_glfw 
		fem ofsolve ofai ofservice visfem util ofmath 
		ivf ivfgle ivfmath ivfimage 
		ofui_glfw gleivf glad newmat imguifd civetweb 
		PNG::PNG JPEG::JPEG ZLIB::ZLIB glfw GLEW::GLEW OpenGL::GL CURL::libcurl
	)
else()
	target_link_libraries(objframe_glfw 
		fem ofsolve ofai ofservice visfem util ofmath 
		ivf ivfgle ivfmath ivfimage 
		ofui_glfw gleivf glad newmat imguifd civetweb 
		PNG::PNG JPEG::JPEG ZLIB::ZLIB glfw GLEW::GLEW OpenGL::GL CURL::libcurl
	)
endif() 
