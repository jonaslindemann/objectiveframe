file(GLOB SOURCE_FILES *.cpp)
file(GLOB INCLUDE_FILES ${PROJECT_SOURCE_DIR}/include/vfem/*.h)

add_library(visfem ${IVF_INCLUDE_DIR} ${INCLUDE_FILES} ${SOURCE_FILES})
target_include_directories(visfem PUBLIC ${PROJECT_SOURCE_DIR}/include/vfem)
target_link_libraries(visfem PUBLIC glm::glm)
set_target_properties(visfem PROPERTIES DEBUG_POSTFIX d)
target_precompile_headers(visfem PRIVATE pch.h)
