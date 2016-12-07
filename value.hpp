//
//  value.hpp
//  Jsonata
//
//  Copyright © 2015-2016 Dsperados (info@dsperados.com). All rights reserved.
//  Licensed under the BSD 3-clause license.
//

#ifndef JSON_VALUE_HPP
#define JSON_VALUE_HPP

#include <boost/variant.hpp>
#include <cstddef>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace json
{
	//! A json value
	class Value
	{
    public:
        //! Generic null value type
        enum class Null;
        
        //! Convenience alias for Json arrays
        using Array = std::vector<Value>;
        
        //! Convenience alias for Json objects
        using Object = std::map<std::string, Value>;
        
        class Iterator;
        class ConstIterator;
        
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
            
            //! Return this, so that Iterator::operator->() can chain onto this operator
            /*! This is necessary, because operator->() will keep on chaining until it finds a pointer.
                In other words it->value() = (it.operator->())->value() */
            const Accessor* operator->() const { return this; }
            
        private:
            //! The iterator pointing to the element data
            boost::variant<boost::recursive_wrapper<Array::iterator>, boost::recursive_wrapper<Object::iterator>> iterator;
        };
        
        //! Represents an element in a Json array or object
        /*! This class is used as the pointee of json iterators */
        class ConstAccessor
        {
        public:
            //! Construct the accessor from an iterator
            ConstAccessor(const ConstIterator& iterator);
            
            //! Return the key of an object key/value pair
            /*! @throw std::runtime_error if the accessor does not point to an object element */
            const std::string& key();
            
            //! Return the array element, or value of object key/value pair this accessor points to
            const Value& value();
            
            //! Return this, so that Iterator::operator->() can chain onto this operator
            /*! This is necessary, because operator->() will keep on chaining until it finds a pointer.
             In other words it->value() = (it.operator->())->value() */
            ConstAccessor* operator->() { return this; }
            
            //! Return this, so that Iterator::operator->() can chain onto this operator
            /*! This is necessary, because operator->() will keep on chaining until it finds a pointer.
             In other words it->value() = (it.operator->())->value() */
            const ConstAccessor* operator->() const { return this; }
            
        private:
            //! The iterator pointing to the element data
            boost::variant<boost::recursive_wrapper<Array::const_iterator>, boost::recursive_wrapper<Object::const_iterator>> iterator;
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
            boost::variant<boost::recursive_wrapper<Array::iterator>, boost::recursive_wrapper<Object::iterator>> iterator;
        };
        
        //! Iterator over a Json value
        class ConstIterator
        {
        public:
            //! Construct the iterator from an array iterator
            ConstIterator(Array::const_iterator iterator);
            
            //! Construct the iterator from an object iterator
            ConstIterator(Object::const_iterator iterator);
            
            //! Increment the iterator
            ConstIterator& operator++();
            
            //! Increment the iterator
            ConstIterator& operator++(int);
            
            //! Dereference the iterator
            ConstAccessor operator*();
            
            //! Dereference the iterator
            ConstAccessor operator->();
            
            //! Return the iterator as a variant
            const auto& asVariant() const { return iterator; }
            
        private:
            //! Variant containing the actual iterator
            boost::variant<boost::recursive_wrapper<Array::const_iterator>, boost::recursive_wrapper<Object::const_iterator>> iterator;
        };
        
	public:

	// Construction

		// Note: a lot of these can be templated away with c++17's constructor template parameter inference
        Value(Null = {}); //!< Construct with a null value (default construction)
		Value(bool boolean); //!< Construct with a boolean value
        Value(short int number); //!< Construct with a number value
		Value(unsigned short int number); //!< Construct with a number value
        Value(int number); //!< Construct with a number value
		Value(unsigned int number); //!< Construct with a number value
		Value(long int number); //!< Construct with a number value
		Value(long unsigned int number); //!< Construct with a number value
		Value(long long int number); //!< Construct with a number value
		Value(long long unsigned int number); //!< Construct with a number value
		Value(double number); //!< Construct with a number value
		Value(long double number); //!< Construct with a number value
		Value(const std::string& string); //!< Construct a string value
		Value(const Array& array); //!< Construct an array value
		Value(const Object& object); //!< Construct an object value

		//! Construct a string value
		/*! @throw std::invalid_argument if the string is a nullptr */
		Value(const char* string);

	// Assignment

		//! Assign a null value
		Value& operator=(Null);

		//! Assign a new boolean value
		template <class T>
		std::enable_if_t<std::is_same<T, bool>::value, Value&> operator=(T boolean)
		{
			data = boolean;
			return *this;
		}

		//! Assign a new number value
		template <class T>
		std::enable_if_t<std::is_integral<T>::value &&
                         std::is_signed<T>::value &&
                        !std::is_same<T, bool>::value, Value&>
        operator=(T number)
		{
			data = static_cast<int64_t>(number);
			return *this;
		}
        
        //! Assign a new number value
        template <class T>
        std::enable_if_t<std::is_integral<T>::value &&
                         std::is_unsigned<T>::value &&
                        !std::is_same<T, bool>::value, Value&>
        operator=(T number)
        {
            data = static_cast<uint64_t>(number);
            return *this;
        }
        
        //! Assign a new number value
        template <class T>
        std::enable_if_t<std::is_floating_point<T>::value, Value&>
        operator=(T number)
        {
            data = static_cast<long double>(number);
            return *this;
        }

		//! Assign a new string value
		/*! @throw std::invalid_argument if string is a nullptr */
		Value& operator=(const char* string);

		//! Assign a new string value
		Value& operator=(const std::string& string);

		//! Assign a new array value
		Value& operator=(const Array& array);

		//! Assign a new object value
		Value& operator=(const Object& object);

	// Predicates

		bool isNull() const; //!< Is this value a null?
		bool isBool() const; //!< Is this value a boolean?
		bool isNumber() const; //!< Is this value a number?
        bool isInteger() const; //!< Is this value an integer number?
        bool isSignedInteger() const; //!< Is this value a signed integer number?
        bool isUnsignedInteger() const; //!< Is this value an unsigned integer number?
        bool isReal() const; //!< Is this value a real number?
		bool isString() const; //!< Is this value a string?
		bool isArray() const; //!< Is this value a array?
		bool isObject() const; //!< Is this value a object?

	// Access

		//! Retrieve the value as a bool
		/*! @throw std::runtime_error if the value is not a boolean */
		bool asBool() const;
        
        //! Retrieve the value as a signed integer
        /*! @throw std::runtime_error if the value is not a number */
        int64_t asSignedInteger() const;
        
        //! Retrieve the value as an unsigned number
        /*! @throw std::runtime_error if the value is not a number */
        uint64_t asUnsignedInteger() const;

		//! Retrieve the value as a real number
		/*! @throw std::runtime_error if the value is not a number */
		long double asReal() const;

		//! Retrieve the value as a string
		/*! @throw std::runtime_error if the value is not a string */
		const std::string& asString() const;
        
        //! Retrieve the value as an array
        /*! @throw std::runtime_error if the value is not an array */
        const Array& asArray() const;
        
        //! Retrieve the value as an object
        /*! @throw std::runtime_error if the value is not an object */
        const Object& asObject() const;

		//! Represent the Json value as a variant
		const auto& asVariant() const { return data; }

		//! Append a value, if this is an array
        /*! Changes the value into an array if it wasn't */
        void append(const Value& value);

		//! Access an element of the value as array
        /*! Changes the value into an array if it wasn't */
        Value& operator[](std::size_t index);
        
        //! Access an element of the value as array, read-only
        /*! @throw std::runtime_error if the value is not an array */
        const Value& operator[](std::size_t index) const;
        
        //! Access an element of the value as object, or return an alternative if the index wasn't found
        Value access(const std::size_t& index, const Value& alternative) const;
        
        //! Access an element of the value as object
        /*! Changes the value into an object if it wasn't */
        Value& operator[](const std::string& key);
        
        //! Access an element of the value as object, read-only
        /*! @throw std::runtime_error if the value is not an object */
        const Value& operator[](const std::string& key) const;
        
        //! Access an element of the value as object, or return an alternative if the key wasn't found
        Value access(const std::string& key, const Value& alternative) const;

		//! Return the size of the value (as an array or object)
		/*! @throw std::runtime_error if the value is neither array nor object */
		std::size_t size() const;

		//! Return whether the value is empty (as an array or object)
		/*! @throw std::runtime_error if the value is neither array nor object */
		bool empty() const;

		//! Retrieve a list of all the keys of the value (as an object)
        /*! @throw std::runtime_error if the value is not an object */
        std::vector<std::string> keys() const;
        
        //! Does the value have an object key?
        /*! @return false if the value is not an object to begin with */
        bool hasKey(const std::string& key) const;

    // Ranged for-loops

        Iterator begin();
        ConstIterator begin() const;
        ConstIterator cbegin() const;
        
        Iterator end();
        ConstIterator end() const;
        ConstIterator cend() const;
        
    public:
        //! Null value (there can exist only one!)
        static const Value null;
        
        //! Generic empty array
        static const Value emptyArray;
        
        //! Generic empty object
        static const Value emptyObject;

	private:
		//! Contains the actual data for this Json value
        boost::variant<Null, bool, int64_t, uint64_t, long double, std::string, boost::recursive_wrapper<Array>, boost::recursive_wrapper<Object>> data;
	};

	//! Compare two values for equality
	bool operator==(const Value& lhs, const Value& rhs);

	//! Compare two values for inequality
	bool operator!=(const Value& lhs, const Value& rhs);
    
    //! Compare two iterators for equality
    bool operator==(const Value::Iterator& lhs, const Value::Iterator& rhs);
    
    //! Compare two iterators for inequality
    bool operator!=(const Value::Iterator& lhs, const Value::Iterator& rhs);
    
    //! Compare two const iterators for equality
    bool operator==(const Value::ConstIterator& lhs, const Value::ConstIterator& rhs);
    
    //! Compare two const iterators for inequality
    bool operator!=(const Value::ConstIterator& lhs, const Value::ConstIterator& rhs);
}

#endif
