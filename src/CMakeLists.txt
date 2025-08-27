
add_executable(alma main.cpp registers.cpp environment.cpp special_operator.cpp function.cpp package.cpp objects.cpp compiler.cpp)
target_include_directories(alma SYSTEM PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/third_party)
target_link_libraries(alma PRIVATE ${KEYSTONE_LIBRARIES})
