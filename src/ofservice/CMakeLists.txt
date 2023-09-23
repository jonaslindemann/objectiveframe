FILE(GLOB SOURCE_FILES *.cpp *.c *.h )

FILE(GLOB INCLUDE_FILES ${PROJECT_SOURCE_DIR}/include/civetweb/*.h ${PROJECT_SOURCE_DIR}/include/civetweb/*.inl ${PROJECT_SOURCE_DIR}/include/ofservice/*.h ${PROJECT_SOURCE_DIR}/src/objframe_glfw/*.h ) 

IF(CMAKE_BUILD_TYPE STREQUAL Debug)
	include_directories(BEFORE ${PROJECT_SOURCE_DIR}/src/objframe_glfw ${PROJECT_SOURCE_DIR}/include)
	add_library(ofserviced ${INCLUDE_FILES} ${SOURCE_FILES})
	target_include_directories(ofserviced PRIVATE ${CHAISCRIPT_INCLUDE_DIRS})
ELSE(CMAKE_BUILD_TYPE STREQUAL Debug)
	include_directories(BEFORE ${PROJECT_SOURCE_DIR}/src/objframe_glfw ${PROJECT_SOURCE_DIR}/include)
	add_library(ofservice ${INCLUDE_FILES} ${SOURCE_FILES})
	target_include_directories(ofservice PRIVATE ${CHAISCRIPT_INCLUDE_DIRS})
ENDIF(CMAKE_BUILD_TYPE STREQUAL Debug)
