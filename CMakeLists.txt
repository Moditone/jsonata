cmake_minimum_required(VERSION 3.5.1)

project(json)

add_definitions(-std=c++1z -Wall)
include_directories(/usr/local/include)

add_library(json SHARED iterator.cpp value.cpp)
install(TARGETS json DESTINATION lib)
install(FILES array.hpp iterator.hpp object.hpp value.hpp DESTINATION include/json)