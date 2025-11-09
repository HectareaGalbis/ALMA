
add_executable(alma main.cpp environment.cpp special_operator.cpp reader.cpp function.cpp macro.cpp symbol.cpp
  package.cpp objects.cpp emitter.cpp)
target_include_directories(alma SYSTEM PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/third_party)
target_link_libraries(alma PRIVATE ${KEYSTONE_LIBRARIES})
