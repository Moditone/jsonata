cmake_minimum_required(VERSION 3.5.1)

project(json)

add_definitions(-std=c++1z -Wall)
include_directories(/usr/local/include)

add_library(json SHARED iterator.cpp parse.cpp value.cpp writer.cpp)
install(TARGETS json DESTINATION lib)
install(FILES array.hpp iterator.hpp json.hpp object.hpp parse.hpp value.hpp writer.hpp DESTINATION include/json)