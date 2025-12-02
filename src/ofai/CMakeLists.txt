file(GLOB SOURCE_FILES *.cpp)
file(GLOB INCLUDE_FILES *.h ${PROJECT_SOURCE_DIR}/include/ofai/*.h)

add_library(ofai ${INCLUDE_FILES} ${SOURCE_FILES})
target_include_directories(ofai PRIVATE ${CURL_INCLUDE_DIRS})
set_target_properties(ofai PROPERTIES DEBUG_POSTFIX d) 
