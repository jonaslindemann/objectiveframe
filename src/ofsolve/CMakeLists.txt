file(GLOB SOURCE_FILES *.cpp)
file(GLOB INCLUDE_FILES ${PROJECT_SOURCE_DIR}/include/ofsolve/*.h)

add_library(ofsolve ${INCLUDE_FILES} ${SOURCE_FILES})
set_target_properties(ofsolve PROPERTIES DEBUG_POSTFIX d)
target_precompile_headers(ofsolve PRIVATE pch.h)
