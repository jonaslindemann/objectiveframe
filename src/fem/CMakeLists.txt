file(GLOB SOURCE_FILES *.cpp)
file(GLOB INCLUDE_FILES ${PROJECT_SOURCE_DIR}/include/ofem/*.h)

add_library(fem ${INCLUDE_FILES} ${SOURCE_FILES})
target_link_libraries(fem PUBLIC nlohmann_json::nlohmann_json)
set_target_properties(fem PROPERTIES DEBUG_POSTFIX d)
target_precompile_headers(fem PRIVATE pch.h)
