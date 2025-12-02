file(GLOB SOURCE_FILES *.cpp)
file(GLOB INCLUDE_FILES *.h) 

include_directories(BEFORE ${PROJECT_SOURCE_DIR}/src/ofai_test ${PROJECT_SOURCE_DIR}/include/ofai)
add_definitions(-DUSE_FEMVIEW)

if (WIN32)
	add_compile_options(/bigobj)
endif()

add_executable(ofai_test ${IVF_INCLUDE_DIR} ${INCLUDE_FILES} ${SOURCE_FILES})
target_include_directories(ofai_test PRIVATE ${CHAISCRIPT_INCLUDE_DIRS})
set_target_properties(ofai_test PROPERTIES DEBUG_POSTFIX d)

if (WIN32)
	# Set WIN32 subsystem only for Release builds
	set_target_properties(ofai_test PROPERTIES 
		WIN32_EXECUTABLE $<CONFIG:Release>
		LINK_FLAGS_RELEASE "/ENTRY:mainCRTStartup /SUBSYSTEM:WINDOWS"
	)
endif()

target_link_libraries(ofai_test ofai util PNG::PNG JPEG::JPEG ZLIB::ZLIB glfw GLEW::GLEW OpenGL::GL CURL::libcurl) 
