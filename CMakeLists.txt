cmake_minimum_required(VERSION 3.0.0)

# Create the project
project(Jsonata)

if(WIN32)
	add_definitions(/std:c++latest /Wall /WX-)
	install(FILES json.hpp parse.hpp value.hpp writer.hpp DESTINATION moditone/jsonata)
endif(WIN32)

if(APPLE)
	set(CMAKE_OSX_DEPLOYMENT_TARGET "10.9" CACHE STRING "Minimum OS X deployment version")
	# Add global definitions and include directories
	add_definitions(-std=c++14 -Wall -Werror -Wconversion)
	include_directories(/usr/local/include)
	install(FILES json.hpp parse.hpp value.hpp writer.hpp DESTINATION include/moditone/jsonata)
endif(APPLE)

# Create the target
add_library(Jsonata accessor.cpp iterator.cpp json.hpp parse.hpp parse.cpp value.hpp value.cpp writer.hpp writer.cpp)

install(TARGETS Jsonata DESTINATION lib)

if (BUILD_SHARED_LIBS)
	set_target_properties(Jsonata PROPERTIES OUTPUT_NAME jsonata)
else (BUILD_SHARED_LIBS)
	set_target_properties(Jsonata PROPERTIES OUTPUT_NAME jsonata_static)
endif (BUILD_SHARED_LIBS)