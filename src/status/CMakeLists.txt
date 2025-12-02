FILE(GLOB SOURCE_FILES *.cpp)
FILE(GLOB INCLUDE_FILES *.h)

# Note: This library has an unusual name (ivf2d) - keeping it for compatibility
ADD_LIBRARY(ivf2d ${INCLUDE_FILES} ${SOURCE_FILES})
set_target_properties(ivf2d PROPERTIES DEBUG_POSTFIX d)
