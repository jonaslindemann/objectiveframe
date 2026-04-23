file(GLOB SOURCE_FILES *.cpp)
file(GLOB INCLUDE_FILES *.h)

add_library(newmat ${INCLUDE_FILES} ${SOURCE_FILES})
set_target_properties(newmat PROPERTIES DEBUG_POSTFIX d)
