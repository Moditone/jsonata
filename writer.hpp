#ifndef JSON_WRITER_HPP
#define JSON_WRITER_HPP

#include <ostream>

#include "Value.hpp"

namespace json
{
	//! Json writer settings, streamable to output
	class Writer
    {
    public:
    	//! Construct the writer with a value
        Writer(const Value& value);
        
    public:
    	//! The value that will be written to output
        Value value;
    };
    
    //! Stream a json writer with settings to an output stream
    std::ostream& operator<<(std::ostream& stream, const Writer& writer);
}

#endif