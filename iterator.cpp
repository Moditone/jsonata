#include <stdexcept>

#include "iterator.hpp"
#include "value.hpp"

using namespace std;

namespace json
{
	Accessor::Accessor(const Iterator& iterator) :
		iterator(iterator.asVariant())
	{

	}

	const string& Accessor::key()
	{
		if (iterator.type() == typeid(Object::iterator))
			return boost::get<Object::iterator>(iterator)->first;

		throw runtime_error("Accessor does not point to an object element, yet key() was called on it");
	}

	Value& Accessor::value()
	{
		switch (iterator.which())
		{
			case 0: return *boost::get<Array::iterator>(iterator);
			case 1: return boost::get<Object::iterator>(iterator)->second;
			default: assert(false);
		}
	}

	Iterator::Iterator(Array::iterator iterator) :
		iterator(iterator)
	{

	}

	Iterator::Iterator(Object::iterator iterator) :
		iterator(iterator)
	{
		
	}

	Iterator& Iterator::operator++()
	{
		switch (iterator.which())
		{
			case 0: ++boost::get<Array::iterator>(iterator); break;
			case 1: ++boost::get<Object::iterator>(iterator); break;
		}

		return *this;
	}

	Iterator& Iterator::operator++(int)
	{
		switch (iterator.which())
		{
			case 0: boost::get<Array::iterator>(iterator)++; break;
			case 1: boost::get<Object::iterator>(iterator)++; break;
		}

		return *this;
	}

	Accessor Iterator::operator*()
	{
		return {*this};
	}

	Accessor Iterator::operator->()
	{
		return {*this};
	}

	bool operator==(const Iterator& lhs, const Iterator& rhs)
	{
		return lhs.asVariant() == rhs.asVariant();
	}

	bool operator!=(const Iterator& lhs, const Iterator& rhs)
	{
		return !(lhs == rhs);	
	}
}