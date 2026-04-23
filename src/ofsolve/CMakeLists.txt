file(GLOB SOURCE_FILES *.cpp)
file(GLOB INCLUDE_FILES ${PROJECT_SOURCE_DIR}/include/ofsolve/*.h)

add_library(ofsolve ${INCLUDE_FILES} ${SOURCE_FILES})
target_link_libraries(ofsolve PUBLIC Eigen3::Eigen nlohmann_json::nlohmann_json Spectra::Spectra)
set_target_properties(ofsolve PROPERTIES DEBUG_POSTFIX d)
target_precompile_headers(ofsolve PRIVATE pch.h)
