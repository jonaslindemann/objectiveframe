file(GLOB SOURCE_FILES *.cpp)
file(GLOB INCLUDE_FILES ${PROJECT_SOURCE_DIR}/include/imguifd/*.h)

add_library(imguifd ${INCLUDE_FILES} ${SOURCE_FILES})
set_target_properties(imguifd PROPERTIES DEBUG_POSTFIX d)
