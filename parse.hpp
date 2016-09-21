#ifndef JSON_PARSE_HPP
#define JSON_PARSE_HPP

#include <istream>

#include "value.hpp"

namespace json
{
	Value parse(std::istream& stream);
}

#endif