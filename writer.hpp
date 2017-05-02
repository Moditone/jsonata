//
//  writer.hpp
//  Jsonata
//
//  Copyright © 2015-2016 Dsperados (info@dsperados.com). All rights reserved.
//  Licensed under the BSD 3-clause license.
//

#ifndef JSON_WRITER_HPP
#define JSON_WRITER_HPP

#include <ostream>
#include <string>

#include "value.hpp"

namespace json
{
    //! Base class for json writers
    class Writer
    {
    public:
        //! Virtual destructor, because this is a polymorphic class
        virtual ~Writer() = default;
        
        //! Write a Json value to stream
        /*! Virtual, so that derivatives can implement their own formatting */
        virtual void writeToStream(std::ostream& stream, const Value& value) const = 0;
        
        //! Write a Json value to string
        std::string writeToString(const Value& value) const;
    };
    
    //! Settings the json writer uses to format its output
    class LeanWriter : public Writer
    {
    public:
        //! Write a Json value to stream, lean and efficient
        void writeToStream(std::ostream& stream, const Value& value) const override;

    public:
        //! The precision for writing real numbers
        std::size_t realPrecision = 10;
        
    protected:
        //! Write a Json string to stream
        void write(std::ostream& stream, const std::string& string) const;
    };
    
    //! Default pretty formatting settings that come with libjsonata
    class PrettyWriter : public LeanWriter
    {
    public:
        //! Write a Json value to stream using pretty formatting
        void writeToStream(std::ostream& stream, const Value& value) const override;
        
    private:
        //! Write a Json value to stream using indentation
        void writeToStreamWithIndentation(std::ostream& stream, const Value& value, unsigned int indentation) const;
        
        //! Output a number of whitespaces for indentation
        void writeIndentation(std::ostream& stream, unsigned int indentation) const;
    };
    
    //! Streams Json through a writer to an output stream
    class Streamer
    {
    public:
        //! Construct the writer by passing a value and format
        /*! @warning The value and writer should outlive the Streamer */
        Streamer(const Value& value, const Writer& writer);
        
    public:
        //! The value that will be outputted
        const Value& value;
        
        //! The writer that is used for outputting
        const Writer& writer;
    };
    
    //! Stream a json Streamer to an output stream
    std::ostream& operator<<(std::ostream& stream, const Streamer& streamer);
    
    //! Stream a json value to an output stream
    std::ostream& operator<<(std::ostream& stream, const Value& value);
}

#endif
