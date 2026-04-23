file(GLOB SOURCE_FILES *.cpp *.c *.h *.inl)
file(GLOB INCLUDE_FILES ${PROJECT_SOURCE_DIR}/include/civetweb/*.h ${PROJECT_SOURCE_DIR}/include/civetweb/*.inl)

add_library(civetweb ${INCLUDE_FILES} ${SOURCE_FILES})
set_target_properties(civetweb PROPERTIES DEBUG_POSTFIX d)
