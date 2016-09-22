cmake_minimum_required(VERSION 3.5.1)

project(jsonata)

add_definitions(-std=c++1z -Wall)
include_directories(/usr/local/include)

add_library(jsonata SHARED
    iterator.cpp
    json.hpp
    parse.hpp parse.cpp
    value.hpp value.cpp
    writer.hpp writer.cpp)

install(TARGETS jsonata DESTINATION lib)
install(FILES json.hpp parse.hpp value.hpp writer.hpp DESTINATION include/jsonata)
