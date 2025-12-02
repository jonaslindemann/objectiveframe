file(GLOB SOURCE_FILES *.cpp)
file(GLOB INCLUDE_FILES *.h)

add_executable(objsolver ${INCLUDE_FILES} ${SOURCE_FILES})
set_target_properties(objsolver PROPERTIES DEBUG_POSTFIX d)
target_link_libraries(objsolver ofsolve fem util) 
