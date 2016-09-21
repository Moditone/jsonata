#ifndef JSON_OBJECT_HPP
#define JSON_OBJECT_HPP

#include <string>
#include <unordered_map>

namespace json
{
	class Value;

	//! Convenience alias for Json objects
	using Object = std::unordered_map<std::string, Value>;
}

#endif