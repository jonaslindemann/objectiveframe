file(GLOB SOURCE_FILES *.cpp)
file(GLOB INCLUDE_FILES *.h)

if(CMAKE_BUILD_TYPE STREQUAL Debug)
	add_executable(objsolverd ${INCLUDE_FILES} ${SOURCE_FILES})
else()
	add_executable(objsolver ${INCLUDE_FILES} ${SOURCE_FILES})
endif()

if(CMAKE_BUILD_TYPE STREQUAL Debug)
	target_link_libraries(objsolverd femd newmatd)
else()
	target_link_libraries(objsolver fem newmat)
endif()
