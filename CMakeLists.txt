cmake_minimum_required(VERSION 3.5.1)

project(jsonata)

add_definitions(-std=c++1z -Wall)
include_directories(/usr/local/include)

add_library(jsonata SHARED iterator.cpp parse.cpp value.cpp writer.cpp
    json.hpp parse.hpp value.hpp writer.hpp)
install(TARGETS jsonata DESTINATION lib)
install(FILES json.hpp parse.hpp value.hpp writer.hpp DESTINATION include/jsonata)
