//
//  value.cpp
//  Jsonata
//
//  Copyright © 2015-2016 Dsperados (info@dsperados.com). All rights reserved.
//  Licensed under the BSD 3-clause license.
//

#include <cmath>
#include <stdexcept>

#include "value.hpp"

using namespace std;

namespace json
{
    const Value Value::null = Value::Null{};
    const Value Value::emptyArray = Value::Array{};
    const Value Value::emptyObject = Value::Object{};
    
    Value::Value(Null) : data(Null{}) { }
	Value::Value(bool boolean) : data(boolean) { }
    Value::Value(short int number) { *this = number; }
    Value::Value(unsigned short int number) { *this = number; }
	Value::Value(int number) { *this = number; }
	Value::Value(unsigned int number) { *this = number; }
	Value::Value(long int number) { *this = number; }
	Value::Value(long unsigned int number) { *this = number; }
	Value::Value(long long int number) { *this = number; }
	Value::Value(long long unsigned int number) { *this = number; }
	Value::Value(double number) { *this = number; }
    Value::Value(long double number) { *this = number; }
	Value::Value(const string& string) : data(string) { }
	Value::Value(const Array& array) : data(array) { }
	Value::Value(const Object& object) : data(object) { }

	Value::Value(const char* string)
	{
		*this = string;
	}
    
    Value::Value(Value&& rhs)
    {
        *this = move(rhs);
    }

	Value& Value::operator=(Null)
	{
        data = Null{};
		return *this;
	}

	Value& Value::operator=(const char* string)
	{
		if (string == nullptr)
			throw invalid_argument("Json value was assigned a nullptr string");

		return *this = std::string(string);
	}

	Value& Value::operator=(const string& string)
	{
		data = string;
		return *this;
	}

	Value& Value::operator=(const Array& array)
	{
		data = array;
		return *this;
	}

	Value& Value::operator=(const Object& object)
	{
		data = object;
		return *this;
	}
    
    Value& Value::operator=(Value&& rhs)
    {
        data = rhs.data;
        rhs = null;
        
        return *this;
    }

	bool Value::isNull() const { return data.type() == typeid(Null); }
	bool Value::isBool() const { return data.type() == typeid(bool); }
    bool Value::isNumber() const { return isInteger() || isReal(); }
    bool Value::isInteger() const { return isSignedInteger() || isUnsignedInteger(); }
    bool Value::isSignedInteger() const { return data.type() == typeid(int64_t); }
    bool Value::isUnsignedInteger() const { return data.type() == typeid(uint64_t); }
    bool Value::isReal() const { return data.type() == typeid(long double); }
	bool Value::isString() const { return data.type() == typeid(string); }
	bool Value::isArray() const { return data.type() == typeid(Array); }
	bool Value::isObject() const { return data.type() == typeid(Object); }

	bool Value::asBool() const
	{
		if (!isBool())
			throw runtime_error("Json value is not a boolean, yet asBool() was called on it");

		return boost::get<bool>(data);
	}
    
    int64_t Value::asSignedInteger() const
    {
        switch (data.which())
        {
            case 2: return boost::get<int64_t>(data);
            case 3: return boost::get<uint64_t>(data);
            case 4: return boost::get<long double>(data);
            default: throw runtime_error("Json value is not a number, yet asNumber() was called on it");
        }
    }
    
    uint64_t Value::asUnsignedInteger() const
    {
        switch (data.which())
        {
            case 2: return boost::get<int64_t>(data);
            case 3: return boost::get<uint64_t>(data);
            case 4: return boost::get<long double>(data);
            default: throw runtime_error("Json value is not a number, yet asNumber() was called on it");
        }
    }

	long double Value::asReal() const
	{
        switch (data.which())
        {
            case 2: return boost::get<int64_t>(data);
            case 3: return boost::get<uint64_t>(data);
            case 4: return boost::get<long double>(data);
            default: throw runtime_error("Json value is not a number, yet asNumber() was called on it");
        }
	}

	const string& Value::asString() const
	{
		if (!isString())
			throw runtime_error("Json value is not a string, yet asString() was called on it");

		return boost::get<string>(data);
	}
    
    const Value::Array& Value::asArray() const
    {
        if (!isArray())
            throw runtime_error("Json value is not an array, yet asArray() was called on it");
        
        return boost::get<Array>(data);
    }
    
    const Value::Object& Value::asObject() const
    {
        if (!isObject())
            throw runtime_error("Json value is not an object, yet asObject() was called on it");
        
        return boost::get<Object>(data);
    }

	void Value::append(const Value& value)
    {
        if (!isArray())
            *this = emptyArray;
        
        boost::get<Array>(data).emplace_back(value);
    }

	Value& Value::operator[](size_t index)
    {
        if (!isArray())
            *this = emptyArray;
        
        if (index >= size())
            throw runtime_error("Json array value, index " + to_string(index) + " out of bounds");
        
        return boost::get<Array>(data).at(index);
    }
    
    const Value& Value::operator[](size_t index) const
    {
        if (!isArray())
            throw runtime_error("Json value is not an array, but tried to call operator[]() on it");
        
        if (index >= size())
            throw runtime_error("Json array, index " + to_string(index) + " out of bounds");
        
        return boost::get<Array>(data).at(index);
    }
    
    Value Value::access(const size_t& index, const Value& alternative) const
    {
        if (index >= size())
            return alternative;
        
        return (*this)[index];
    }
    
    Value& Value::operator[](const string& key)
    {
        if (!isObject())
            *this = emptyObject;
        
        return boost::get<Object>(data)[key];
    }
    
    const Value& Value::operator[](const string& key) const
    {
        if (!isObject())
            throw runtime_error("Json value is not an object, but tried to call operator[]() on it");
        
        auto& object = boost::get<Object>(data);
        auto it = object.find(key);
        if (it == object.end())
            throw runtime_error("Json object, key '" + key + "' not found");
        
        return it->second;
    }
    
    Value Value::access(const string& key, const Value& alternative) const
    {
        if (!hasKey(key))
            return alternative;
        
        return (*this)[key];
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
    
    bool Value::hasKey(const string& key) const
    {
        if (!isObject())
            return false;
        
        return boost::get<Object>(data).count(key) > 0;
    }

    Value::Iterator Value::begin()
    {
    	if (isArray())
            return boost::get<Array>(data).begin();
        else if (isObject())
            return boost::get<Object>(data).begin();
        else
            throw runtime_error("Json value is neither array nor object, but tried to call begin() on it");
    }
    
    Value::ConstIterator Value::begin() const
    {
        if (isArray())
            return boost::get<Array>(data).begin();
        else if (isObject())
            return boost::get<Object>(data).begin();
        else
            throw runtime_error("Json value is neither array nor object, but tried to call begin() on it");
    }
    
    Value::ConstIterator Value::cbegin() const
    {
        if (isArray())
            return boost::get<Array>(data).begin();
        else if (isObject())
            return boost::get<Object>(data).begin();
        else
            throw runtime_error("Json value is neither array nor object, but tried to call begin() on it");
    }

    Value::Iterator Value::end()
    {
    	if (isArray())
            return boost::get<Array>(data).end();
        else if (isObject())
            return boost::get<Object>(data).end();
        else
            throw runtime_error("Json value is neither array nor object, but tried to call end() on it");
    }
    
    Value::ConstIterator Value::end() const
    {
        if (isArray())
            return boost::get<Array>(data).end();
        else if (isObject())
            return boost::get<Object>(data).end();
        else
            throw runtime_error("Json value is neither array nor object, but tried to call end() on it");
    }
    
    Value::ConstIterator Value::cend() const
    {
        if (isArray())
            return boost::get<Array>(data).end();
        else if (isObject())
            return boost::get<Object>(data).end();
        else
            throw runtime_error("Json value is neither array nor object, but tried to call end() on it");
    }

	bool operator==(const Value& lhs, const Value& rhs)
	{
		return lhs.asVariant() == rhs.asVariant();
	}

	bool operator!=(const Value& lhs, const Value& rhs)
	{
		return !(lhs == rhs);
	}
}
