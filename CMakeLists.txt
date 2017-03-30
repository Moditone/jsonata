cmake_minimum_required(VERSION 3.0.0)

# Create the project
project(Jsonata)

# Add global definitions and include directories
add_definitions(-std=c++14 -Wall -Werror)
include_directories(/usr/local/include)

# Create the target
add_library(Jsonata accessor.cpp iterator.cpp json.hpp parse.hpp parse.cpp value.hpp value.cpp writer.hpp writer.cpp)

install(TARGETS Jsonata DESTINATION lib)
install(FILES json.hpp parse.hpp value.hpp writer.hpp DESTINATION include/jsonata)

if (BUILD_SHARED_LIBS)
	set_target_properties(Jsonata PROPERTIES OUTPUT_NAME jsonata)
else (BUILD_SHARED_LIBS)
	set_target_properties(Jsonata PROPERTIES OUTPUT_NAME jsonata_static)
endif (BUILD_SHARED_LIBS)