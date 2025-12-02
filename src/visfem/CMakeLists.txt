FILE(GLOB SOURCE_FILES *.cpp) 
FILE(GLOB INCLUDE_FILES ${PROJECT_SOURCE_DIR}/include/vfem/*.h) 

ADD_LIBRARY(visfem ${IVF_INCLUDE_DIR} ${INCLUDE_FILES} ${SOURCE_FILES})
target_include_directories(visfem PUBLIC ${PROJECT_SOURCE_DIR}/include/vfem)
set_target_properties(visfem PROPERTIES DEBUG_POSTFIX d) 
