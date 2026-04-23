file(GLOB SOURCE_FILES *.cpp)
file(GLOB INCLUDE_FILES ${PROJECT_SOURCE_DIR}/include/ofutil/*.h)

add_library(util ${INCLUDE_FILES} ${SOURCE_FILES})
target_include_directories(util PUBLIC ${PROJECT_SOURCE_DIR}/include/ofutil)
set_target_properties(util PROPERTIES DEBUG_POSTFIX d)
