FILE(GLOB SOURCE_FILES *.cpp *.c *.h )

FILE(GLOB INCLUDE_FILES ${PROJECT_SOURCE_DIR}/include/civetweb/*.h ${PROJECT_SOURCE_DIR}/include/civetweb/*.inl ${PROJECT_SOURCE_DIR}/include/ofservice/*.h ${PROJECT_SOURCE_DIR}/src/objframe_glfw/*.h ) 

include_directories(BEFORE ${PROJECT_SOURCE_DIR}/src/objframe_glfw ${PROJECT_SOURCE_DIR}/include)
add_library(ofservice ${INCLUDE_FILES} ${SOURCE_FILES})
target_include_directories(ofservice PRIVATE ${CHAISCRIPT_INCLUDE_DIRS})
set_target_properties(ofservice PROPERTIES DEBUG_POSTFIX d)
