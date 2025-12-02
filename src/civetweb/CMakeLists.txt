FILE(GLOB SOURCE_FILES *.cpp *.c *.h *.inl )

FILE(GLOB INCLUDE_FILES ${PROJECT_SOURCE_DIR}/include/civetweb/*.h ${PROJECT_SOURCE_DIR}/include/civetweb/*.inl ) 

ADD_LIBRARY(civetweb ${INCLUDE_FILES} ${SOURCE_FILES})
set_target_properties(civetweb PROPERTIES DEBUG_POSTFIX d)
