#include <stdexcept>

#include "value.hpp"

using namespace std;

namespace json
{
    Value::Accessor::Accessor(const Value::Iterator& iterator) :
		iterator(iterator.asVariant())
	{

	}

	const string& Value::Accessor::key()
	{
		if (iterator.type() == typeid(Object::iterator))
			return boost::get<Object::iterator>(iterator)->first;

		throw runtime_error("Accessor does not point to an object element, yet key() was called on it");
	}

	Value& Value::Accessor::value()
	{
		switch (iterator.which())
		{
			case 0: return *boost::get<Array::iterator>(iterator);
			case 1: return boost::get<Object::iterator>(iterator)->second;
			default: assert(false);
		}
	}

	Value::Iterator::Iterator(Array::iterator iterator) :
		iterator(iterator)
	{

	}

	Value::Iterator::Iterator(Object::iterator iterator) :
		iterator(iterator)
	{
		
	}

	Value::Iterator& Value::Iterator::operator++()
	{
		switch (iterator.which())
		{
			case 0: ++boost::get<Array::iterator>(iterator); break;
			case 1: ++boost::get<Object::iterator>(iterator); break;
		}

		return *this;
	}

	Value::Iterator& Value::Iterator::operator++(int)
	{
		switch (iterator.which())
		{
			case 0: boost::get<Array::iterator>(iterator)++; break;
			case 1: boost::get<Object::iterator>(iterator)++; break;
		}

		return *this;
	}

	Value::Accessor Value::Iterator::operator*()
	{
		return {*this};
	}

	Value::Accessor Value::Iterator::operator->()
	{
		return {*this};
	}

	bool operator==(const Value::Iterator& lhs, const Value::Iterator& rhs)
	{
		return lhs.asVariant() == rhs.asVariant();
	}

	bool operator!=(const Value::Iterator& lhs, const Value::Iterator& rhs)
	{
		return !(lhs == rhs);	
	}
}
