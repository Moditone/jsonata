//
//  value.cpp
//  Jsonata
//
//  Copyright © 2015-2016 Dsperados (info@dsperados.com). All rights reserved.
//  Licensed under the BSD 3-clause license.
//

#include <cmath>
#include <stdexcept>
#include <utility>

#include "value.hpp"

using namespace std;

namespace json
{
    const Value Value::null = Value::Null{};
    const Value Value::emptyArray = Value::Array{};
    const Value Value::emptyObject = Value::Object{};
    
    Value::Value() : type(Type::NIL) { }
    Value::Value(Null) { *this = null; }
    Value::Value(bool boolean) { *this = boolean; }
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
    Value::Value(const std::string& string) { *this = string; }
    Value::Value(const Array& array) { *this = array; }
    Value::Value(Array&& array) { *this = std::move(array); }
    Value::Value(const Object& object) { *this = object; }
    Value::Value(Object&& object) { *this = std::move(object); }

	Value::Value(const char* string)
	{
		*this = string;
	}
    
    Value::Value(const Value& rhs)
    {
        *this = rhs;
    }
    
    Value::Value(Value&& rhs)
    {
        *this = std::move(rhs);
    }
    
    Value::~Value()
    {
        destruct();
    }

	Value& Value::operator=(Null)
	{
        destruct();
        type = Type::NIL;
        
		return *this;
	}
    
    Value& Value::operator=(bool boolean)
    {
        destruct();
        type = Type::BOOLEAN;
        this->boolean = boolean;
        
        return *this;
    }

	Value& Value::operator=(const char* string)
	{
		return *this = std::string(string);
	}

    Value& Value::operator=(const std::string& string)
	{
        destruct();
        type = Type::STRING;
        new (&this->string) std::string(string);
        
		return *this;
	}

	Value& Value::operator=(const Array& array)
	{
        destruct();
        type = Type::ARRAY;
        new (&this->array) Array();
        this->array.reserve(array.size());
        for (auto& item : array)
            this->array.emplace_back(std::make_unique<Value>(*item));
        
		return *this;
	}
    
    Value& Value::operator=(Array&& array)
    {
        destruct();
        type = Type::ARRAY;
        new (&this->array) Array(std::move(array));
        
        return *this;
    }

	Value& Value::operator=(const Object& object)
	{
        destruct();
        type = Type::OBJECT;
        new (&this->object) Object();
        for (auto& pair : object)
            this->object.emplace(pair.first, make_unique<Value>(*pair.second));
        
		return *this;
	}
    
    Value& Value::operator=(Object&& object)
    {
        destruct();
        type = Type::OBJECT;
        new (&this->object) Object(std::move(object));
        
        return *this;
    }
    
    Value& Value::operator=(const Value& rhs)
    {
        destruct();
        
        type = rhs.type;
        switch (type)
        {
            case Type::NIL: break;
            case Type::BOOLEAN: boolean = rhs.boolean; break;
            case Type::SIGNED: signedInt = rhs.signedInt; break;
            case Type::UNSIGNED: unsignedInt = rhs.unsignedInt; break;
            case Type::REAL: real = rhs.real; break;
            case Type::STRING: new (&string) std::string(rhs.string); break;
            case Type::ARRAY: *this = rhs.array; break;
            case Type::OBJECT: *this = rhs.object; break;
        }
        
        return *this;
    }
    
    Value& Value::operator=(Value&& rhs)
    {
        destruct();
        
        type = rhs.type;
        switch (type)
        {
            case Type::NIL: break;
            case Type::BOOLEAN: boolean = rhs.boolean; break;
            case Type::SIGNED: signedInt = rhs.signedInt; break;
            case Type::UNSIGNED: unsignedInt = rhs.unsignedInt; break;
            case Type::REAL: real = rhs.real; break;
            case Type::STRING: new (&string) std::string(std::move(rhs.string)); break;
            case Type::ARRAY: *this = std::move(rhs.array); break;
            case Type::OBJECT: *this = std::move(rhs.object); break;
        }
        
        rhs = null;
        
        return *this;
    }

    bool Value::isNull() const { return type == Type::NIL; }
	bool Value::isBool() const { return type == Type::BOOLEAN; }
    bool Value::isNumber() const { return isInteger() || isReal(); }
    bool Value::isInteger() const { return isSignedInteger() || isUnsignedInteger(); }
    bool Value::isSignedInteger() const { return type == Type::SIGNED; }
    bool Value::isUnsignedInteger() const { return type == Type::UNSIGNED; }
    bool Value::isReal() const { return type == Type::REAL; }
	bool Value::isString() const { return type == Type::STRING; }
	bool Value::isArray() const { return type == Type::ARRAY; }
	bool Value::isObject() const { return type == Type::OBJECT; }

	bool Value::asBool() const
	{
		if (!isBool())
			throw runtime_error("Json value is not a boolean, yet asBool() was called on it");

        return boolean;
	}
    
    int64_t Value::asSignedInteger() const
    {
        switch (type)
        {
            case Type::SIGNED: return signedInt;
            case Type::UNSIGNED: return static_cast<int64_t>(unsignedInt);
            case Type::REAL: return static_cast<int64_t>(real);
                
            case Type::NIL:
            case Type::BOOLEAN:
            case Type::STRING:
            case Type::ARRAY:
            case Type::OBJECT:
                default: throw runtime_error("Json value is not a number, yet asSignedInteger() was called on it");
        }
    }
    
    uint64_t Value::asUnsignedInteger() const
    {
        switch (type)
        {
            case Type::SIGNED: return static_cast<uint64_t>(signedInt);
            case Type::UNSIGNED: return unsignedInt;
            case Type::REAL: return static_cast<uint64_t>(real);
                
            case Type::NIL:
            case Type::BOOLEAN:
            case Type::STRING:
            case Type::ARRAY:
            case Type::OBJECT:
            default: throw runtime_error("Json value is not a number, yet asUnsignedInteger() was called on it");
        }
    }

	long double Value::asReal() const
	{
        switch (type)
        {
            case Type::SIGNED: return static_cast<long double>(signedInt);
            case Type::UNSIGNED: return static_cast<long double>(unsignedInt);
            case Type::REAL: return real;
                
            case Type::NIL:
            case Type::BOOLEAN:
            case Type::STRING:
            case Type::ARRAY:
            case Type::OBJECT:
            default: throw runtime_error("Json value is not a number, yet asReal() was called on it");
        }
	}

	const string& Value::asString() const
	{
		if (!isString())
			throw runtime_error("Json value is not a string, yet asString() was called on it");

        return string;
	}
    
    const Value::Array& Value::asArray() const
    {
        if (!isArray())
            throw runtime_error("Json value is not an array, yet asArray() was called on it");
        
        return array;
    }
    
    const Value::Object& Value::asObject() const
    {
        if (!isObject())
            throw runtime_error("Json value is not an object, yet asObject() was called on it");
        
        return object;
    }

	void Value::append(const Value& value)
    {
        if (!isArray())
            *this = emptyArray;
        
        array.emplace_back(make_unique<Value>(value));
    }

	Value& Value::operator[](size_t index)
    {
        if (!isArray())
            *this = emptyArray;
        
        if (index >= size())
            throw runtime_error("Json array value, index " + to_string(index) + " out of bounds");
        
        return *array.at(index);
    }
    
    const Value& Value::operator[](size_t index) const
    {
        if (!isArray())
            throw runtime_error("Json value is not an array, but tried to call operator[]() on it");
        
        if (index >= size())
            throw runtime_error("Json array, index " + to_string(index) + " out of bounds");
        
        return *array.at(index);
    }
    
    Value Value::access(const size_t& index, const Value& alternative) const
    {
        if (index >= size())
            return alternative;
        
        return (*this)[index];
    }
    
    Value& Value::operator[](const std::string& key)
    {
        if (!isObject())
            *this = emptyObject;
        
        return *(object)[key];
    }
    
    const Value& Value::operator[](const std::string& key) const
    {
        if (!isObject())
            throw runtime_error("Json value is not an object, but tried to call operator[]() on it");
        
        auto it = object.find(key);
        if (it == object.end())
            throw runtime_error("Json object, key '" + key + "' not found");
        
        return *it->second;
    }
    
    Value Value::access(const std::string& key, const Value& alternative) const
    {
        if (!hasKey(key))
            return alternative;
        
        return (*this)[key];
    }

	size_t Value::size() const
	{
		if (isArray())
            return array.size();
        else if (isObject())
            return object.size();
        else
            throw runtime_error("Json value is neither array nor object, but tried to call size() on it");
	}

	bool Value::empty() const
	{
		if (isArray())
            return array.empty();
        else if (isObject())
            return object.empty();
        else
            throw runtime_error("Json value is neither array nor object, but tried to call empty() on it");
	}

	vector<string> Value::keys() const
    {
        if (!isObject())
            throw runtime_error("Json value is not an object, but tried to call keys() on it");
        
        vector<std::string> keys;
        for (auto& pair : object)
            keys.emplace_back(pair.first);

        return keys;
    }
    
    bool Value::hasKey(const std::string& key) const
    {
        if (!isObject())
            return false;
        
        return object.count(key) > 0;
    }

    Value::Iterator Value::begin()
    {
    	if (isArray())
            return array.begin();
        else if (isObject())
            return object.begin();
        else
            throw runtime_error("Json value is neither array nor object, but tried to call begin() on it");
    }
    
    Value::ConstIterator Value::begin() const
    {
        if (isArray())
            return array.cbegin();
        else if (isObject())
            return object.cbegin();
        else
            throw runtime_error("Json value is neither array nor object, but tried to call begin() on it");
    }
    
    Value::ConstIterator Value::cbegin() const
    {
        if (isArray())
            return array.cbegin();
        else if (isObject())
            return object.cbegin();
        else
            throw runtime_error("Json value is neither array nor object, but tried to call begin() on it");
    }

    Value::Iterator Value::end()
    {
    	if (isArray())
            return array.end();
        else if (isObject())
            return object.end();
        else
            throw runtime_error("Json value is neither array nor object, but tried to call end() on it");
    }
    
    Value::ConstIterator Value::end() const
    {
        if (isArray())
            return array.cend();
        else if (isObject())
            return object.cend();
        else
            throw runtime_error("Json value is neither array nor object, but tried to call end() on it");
    }
    
    Value::ConstIterator Value::cend() const
    {
        if (isArray())
            return array.cend();
        else if (isObject())
            return object.cend();
        else
            throw runtime_error("Json value is neither array nor object, but tried to call end() on it");
    }
    
    void Value::destruct()
    {
        switch (type)
        {
            case Type::NIL:
            case Type::BOOLEAN:
            case Type::SIGNED:
            case Type::UNSIGNED:
            case Type::REAL:
                break;
            case Type::STRING:
                string.~basic_string();
                break;
            case Type::ARRAY:
                array.~Array();
                break;
            case Type::OBJECT:
                object.~Object();
                break;
        }
    }

	bool operator==(const Value& lhs, const Value& rhs)
	{
        if (lhs.type != rhs.type)
            return false;
        
        switch (lhs.type)
        {
            case Value::Type::NIL: return true;
            case Value::Type::BOOLEAN: return lhs.boolean == rhs.boolean;
            case Value::Type::UNSIGNED: return lhs.unsignedInt == rhs.unsignedInt;
            case Value::Type::SIGNED: return lhs.signedInt == rhs.signedInt;
            case Value::Type::REAL: return lhs.real == rhs.real;
            case Value::Type::STRING: return lhs.string == rhs.string;
            case Value::Type::ARRAY: return lhs.array == rhs.array;
            case Value::Type::OBJECT: return lhs.object == rhs.object;
        }
	}

	bool operator!=(const Value& lhs, const Value& rhs)
	{
		return !(lhs == rhs);
	}
}
