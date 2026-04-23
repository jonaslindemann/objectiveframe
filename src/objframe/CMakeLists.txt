file(GLOB SOURCE_FILES *.cpp)
file(GLOB INCLUDE_FILES *.h)

set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/appicon.rc")

include_directories(BEFORE ${PROJECT_SOURCE_DIR}/src/objframe ${PROJECT_SOURCE_DIR}/include/ofui)

if (WIN32)
	add_compile_options(/bigobj)
endif()

add_executable(objframe ${IVF_INCLUDE_DIR} ${INCLUDE_FILES} ${SOURCE_FILES} ${APP_ICON_RESOURCE_WINDOWS})
if (WIN32)
	set_target_properties(objframe PROPERTIES
		WIN32_EXECUTABLE $<CONFIG:Release>
		LINK_FLAGS_RELEASE "/ENTRY:mainCRTStartup /SUBSYSTEM:WINDOWS"
	)
endif()

target_link_libraries(objframe
	chaiscript_headers
	fem ofsolve ofai ofservice visfem util ofmath
	ivf ivfgle ivfmath ivfimage
	ofui gleivf glad newmat imguifd civetweb
	PNG::PNG JPEG::JPEG ZLIB::ZLIB glfw GLEW::GLEW OpenGL::GL CURL::libcurl
)

set_target_properties(objframe PROPERTIES DEBUG_POSTFIX d)
