file(GLOB SOURCE_FILES *.cpp)
file(GLOB INCLUDE_FILES ${PROJECT_SOURCE_DIR}/include/ofmath/*.h)

add_library(ofmath ${INCLUDE_FILES} ${SOURCE_FILES})
target_link_libraries(ofmath PUBLIC glm::glm)
set_target_properties(ofmath PROPERTIES DEBUG_POSTFIX d)
