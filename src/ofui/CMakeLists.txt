file(GLOB SOURCE_FILES *.cpp)
file(GLOB INCLUDE_FILES ${PROJECT_SOURCE_DIR}/include/ofui/*.h)

add_library(ofui ${INCLUDE_FILES} ${SOURCE_FILES})
target_compile_definitions(ofui PUBLIC USE_FEMVIEW)
target_include_directories(ofui PRIVATE
    ${PROJECT_SOURCE_DIR}/src/objframe
    ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(ofui PRIVATE chaiscript_headers)
set_target_properties(ofui PROPERTIES DEBUG_POSTFIX d)
target_precompile_headers(ofui PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/pch.h")
