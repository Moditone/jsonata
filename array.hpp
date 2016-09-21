#ifndef JSON_ARRAY_HPP
#define JSON_ARRAY_HPP

#include <vector>

namespace json
{
	class Value;

	//! Convenience alias for Json arrays
	using Array = std::vector<Value>;
}

#endif