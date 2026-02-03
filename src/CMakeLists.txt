
add_executable(alma main.cpp alma.cpp garbage-collector.cpp environment.cpp reader.cpp package.cpp
  object.cpp symbol.cpp procedure.cpp cons.cpp string.cpp integer.cpp
  function.cpp)
target_include_directories(alma SYSTEM PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/third_party)
target_link_libraries(alma PRIVATE ${KEYSTONE_LIBRARIES})
