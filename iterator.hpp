#ifndef JSON_ITERATOR_HPP
#define JSON_ITERATOR_HPP

#include <boost/variant.hpp>

#include "array.hpp"
#include "object.hpp"

namespace json
{
	class Iterator;
	class Value;

	//! Represents an element in a Json array or object
	/*! This class is used as the pointee of json iterators */
	class Accessor
	{
	public:
		//! Construct the accessor from an iterator
		Accessor(const Iterator& iterator);

		//! Return the key of an object key/value pair
		/*! @throw std::runtime_error if the accessor does not point to an object element */
		const std::string& key();

		//! Return the array element, or value of object key/value pair this accessor points to
		Value& value();

		//! Return this, so that Iterator::operator->() can chain onto this operator
		/*! This is necessary, because operator->() will keep on chaining until it finds a pointer.
			In other words it->value() = (it.operator->())->value() */
		Accessor* operator->() { return this; }

	private:
		//! The iterator pointing to the element data
		boost::variant<Array::iterator, Object::iterator> iterator;
	};

	//! Iterator over a Json value
	class Iterator
	{
	public:
		//! Construct the iterator from an array iterator
		Iterator(Array::iterator iterator);

		//! Construct the iterator from an object iterator
		Iterator(Object::iterator iterator);

		//! Increment the iterator
		Iterator& operator++();

		//! Increment the iterator
		Iterator& operator++(int);

		//! Dereference the iterator
		Accessor operator*();

		//! Dereference the iterator
		Accessor operator->();

		//! Return the iterator as a variant
		const auto& asVariant() const { return iterator; }

	private:
		//! Variant containing the actual iterator
		boost::variant<Array::iterator, Object::iterator> iterator;
	};

	//! Compare two iterators for equality
	bool operator==(const Iterator& lhs, const Iterator& rhs);

	//! Compare two iterators for inequality
	bool operator!=(const Iterator& lhs, const Iterator& rhs);
}

#endif