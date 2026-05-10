file(GLOB SOURCE_FILES *.cpp *.c *.h)
file(GLOB INCLUDE_FILES
    ${PROJECT_SOURCE_DIR}/include/civetweb/*.h
    ${PROJECT_SOURCE_DIR}/include/civetweb/*.inl
    ${PROJECT_SOURCE_DIR}/include/ofservice/*.h)

include_directories(BEFORE ${PROJECT_SOURCE_DIR}/include)
add_library(ofservice ${INCLUDE_FILES} ${SOURCE_FILES})
target_link_libraries(ofservice PRIVATE chaiscript_headers)
set_target_properties(ofservice PROPERTIES DEBUG_POSTFIX d)
target_precompile_headers(ofservice PRIVATE pch.h)
