#ifndef JSON_HPP
#define JSON_HPP

#include <boost/variant.hpp>
#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

namespace json
{
	//! Generic type for null (there can exist only one!)
	enum class Null { null };

	//! Forward declaration for Value (for use with Array and Object)
	class Value;

	//! Convenience alias for Json arrays
	using Array = std::vector<Value>;

	//! Convenience alias for Json objects
	using Object = std::unordered_map<std::string, Value>;

	//! A json value
	class Value
	{
	public:

	// Predicates

		bool isNull() const; //!< Is this value a null?
		bool isBool() const; //!< Is this value a boolean?
		bool isNumber() const; //!< Is this value a number?
		bool isString() const; //!< Is this value a string?
		bool isArray() const; //!< Is this value a array?
		bool isObject() const; //!< Is this value a object?

	// Retrieval

		//! Retrieve the value as a bool
		/*! @throw std::runtime_error if the value is not a boolean */
		bool asBool() const;

		//! Retrieve the value as a number
		/*! @throw std::runtime_error if the value is not a number */
		long double asNumber() const;

		//! Retrieve the value as a string
		/*! @throw std::runtime_error if the value is not a string */
		const std::string& asString() const;

		//! Represent the Json value as a variant
		const auto& asVariant() const { return data; }

		//! Access an element of the value (as an array)
        /*! @throw std::runtime_error if the value is not an array */
        Value& operator[](std::size_t index);
        
        //! Access an element of the value (as an array), read-only
        /*! @throw std::runtime_error if the value is not an array */
        const Value& operator[](std::size_t index) const;
        
        //! Access a key/value pair of the value (as an object)
        /*! @throw std::runtime_error if the value is not an object */
        Value& operator[](const std::string& key);
        
        //! Access a key/value pair of the value (as an object), read-only
        /*! @throw std::runtime_error if the value is not an object */
        const Value& operator[](const std::string& key) const;

		//! Return the size of the value (as an array or object)
		/*! @throw std::runtime_error if the value is neither array nor object */
		std::size_t size() const;

		//! Return whether the value is empty (as an array or object)
		/*! @throw std::runtime_error if the value is neither array nor object */
		bool empty() const;

		//! Retrieve a list of all the keys of the value (as an object)
        /*! @throw std:runtime_error if the value is not an object */
        std::vector<std::string> keys() const;

	private:
		//! Contains the actual data for this Json value
		boost::variant<Null, bool, long double, std::string, Array, Object> data;
	};

	//! Compare two values for equality
	bool operator==(const Value& lhs, const Value& rhs);

	//! Compare two values for inequality
	bool operator!=(const Value& lhs, const Value& rhs);
}

#endif // JSON_HPP