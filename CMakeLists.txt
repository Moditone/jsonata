cmake_minimum_required(VERSION 3.0.0)

if (APPLE)
	# NEEDS TO BE DONE BEFORE project()
	set(CMAKE_OSX_DEPLOYMENT_TARGET "10.9" CACHE STRING "Minimum OS X deployment version")
endif (APPLE)

# Create the project
project(Jsonata)

if(WIN32)
	add_definitions(/std:c++latest /Wall /WX-)
	install(FILES error.hpp json.hpp parse.hpp value.hpp writer.hpp DESTINATION moditone/jsonata)
endif(WIN32)

if(APPLE)
	# Add global definitions and include directories
	add_definitions(-std=c++17 -Wall -Werror -Wconversion)
	include_directories(/usr/local/include)
	install(FILES error.hpp json.hpp lexer.hpp parse.hpp parser.hpp token.hpp value.hpp writer.hpp DESTINATION include/moditone/jsonata)
endif(APPLE)

# Create the target
add_library(Jsonata accessor.cpp iterator.cpp error.hpp error.cpp json.hpp lexer.hpp lexer.cpp parse.hpp parse.cpp parser.hpp parser.cpp token.hpp value.hpp value.cpp writer.hpp writer.cpp)
set_target_properties(Jsonata PROPERTIES DEBUG_POSTFIX -d)

install(TARGETS Jsonata DESTINATION lib)

if (BUILD_SHARED_LIBS)
	set_target_properties(Jsonata PROPERTIES OUTPUT_NAME jsonata)
else (BUILD_SHARED_LIBS)
	set_target_properties(Jsonata PROPERTIES OUTPUT_NAME jsonata_static)
endif (BUILD_SHARED_LIBS)
