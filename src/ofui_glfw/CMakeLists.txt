file(GLOB SOURCE_FILES ../ofui/*.cpp) 
file(GLOB INCLUDE_FILES ${PROJECT_SOURCE_DIR}/include/ofui/*.h ) 

include_directories(BEFORE ${PROJECT_SOURCE_DIR}/src/objframe_glfw ${PROJECT_SOURCE_DIR}/include)
add_definitions(-DUSE_FEMVIEW)
add_library(ofui_glfw ${INCLUDE_FILES} ${SOURCE_FILES})
target_include_directories(ofui_glfw PRIVATE ${CHAISCRIPT_INCLUDE_DIRS})
set_target_properties(ofui_glfw PROPERTIES DEBUG_POSTFIX d)
