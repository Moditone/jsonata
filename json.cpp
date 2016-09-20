#include <stdexcept>

#include "json.hpp"

using namespace std;

namespace json
{
	bool Value::isNull() const { return data.type() == typeid(Null); }
	bool Value::isBool() const { return data.type() == typeid(bool); }
	bool Value::isNumber() const { return data.type() == typeid(long double); }
	bool Value::isString() const { return data.type() == typeid(string); }
	bool Value::isArray() const { return data.type() == typeid(Array); }
	bool Value::isObject() const { return data.type() == typeid(Object); }

	bool Value::asBool() const
	{
		if (!isBool())
			throw runtime_error("Json value is no boolean, yet asBool() was called on it");

		return boost::get<bool>(data);
	}

	long double Value::asNumber() const
	{
		if (!isNumber())
			throw runtime_error("Json value is no boolean, yet asNumber() was called on it");

		return boost::get<long double>(data);
	}

	const string& Value::asString() const
	{
		if (!isString())
			throw runtime_error("Json value is no boolean, yet asString() was called on it");

		return boost::get<string>(data);
	}

	Value& Value::operator[](size_t index)
    {
        if (!isArray())
            throw runtime_error("Json value is not an array, but tried to call operator[]() on it");
        
        return boost::get<Array>(data).at(index);
    }
    
    const Value& Value::operator[](size_t index) const
    {
        if (!isArray())
            throw runtime_error("Json value is not an array, but tried to call operator[]() on it");
        
        return boost::get<Array>(data).at(index);
    }
    
    Value& Value::operator[](const string& key)
    {
        if (!isObject())
            throw runtime_error("Json value is not an object, but tried to call operator[]() on it");
        
        return boost::get<Object>(data)[key];
    }
    
    const Value& Value::operator[](const string& key) const
    {
        if (!isObject())
            throw runtime_error("Json value is not an object, but tried to call operator[]() on it");
        
        return boost::get<Object>(data).at(key);
    }

	size_t Value::size() const
	{
		if (isArray())
            return boost::get<Array>(data).size();
        else if (isObject())
            return boost::get<Object>(data).size();
        else
            throw runtime_error("Json value is neither array nor object, but tried to call size() on it");
	}

	bool Value::empty() const
	{
		if (isArray())
            return boost::get<Array>(data).empty();
        else if (isObject())
            return boost::get<Object>(data).empty();
        else
            throw runtime_error("Json value is neither array nor object, but tried to call empty() on it");
	}

	vector<string> Value::keys() const
    {
        if (!isObject())
            throw runtime_error("Json value is not an object, but tried to call keys() on it");
        
        vector<string> keys;
        for (auto& pair : boost::get<Object>(data))
            keys.emplace_back(pair.first);
        
        return keys;
    }

	bool operator==(const Value& lhs, const Value& rhs)
	{
		return lhs.asVariant() == rhs.asVariant();
	}

	//! Compare two values for inequality
	bool operator!=(const Value& lhs, const Value& rhs)
	{
		return !(lhs == rhs);
	}
}