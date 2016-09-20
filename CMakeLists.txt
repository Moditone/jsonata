cmake_minimum_required(VERSION 3.5.1)

project(json)

add_definitions(-std=c++1z -Wall)

add_library(json SHARED json.cpp)
install(TARGETS json DESTINATION lib)
install(FILES json.hpp DESTINATION include/json)