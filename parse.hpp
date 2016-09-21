#ifndef JSON_PARSE_HPP
#define JSON_PARSE_HPP

#include <istream>

#include "value.hpp"

namespace json
{
	//! Parse a Json value from stream
	/*! @throw std::runtime_error in case of parsing errors */
	Value parse(std::istream& stream);
}

#endif