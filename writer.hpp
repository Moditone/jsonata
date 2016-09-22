#ifndef JSON_WRITER_HPP
#define JSON_WRITER_HPP

#include <ostream>
#include <string>

#include "Value.hpp"

namespace json
{
    //! Json writer, streamable to output
    class Writer
    {
    public:
        //! Settings the json writer uses to format its output
        class Format
        {
        public:
            //! Write a Json value to stream
            /*! Virtual, so that derivatives can implement their own formatting */
            virtual void write(std::ostream& stream, const Value& value) const;
            
        private:
            //! Write a Json string to stream
            void write(std::ostream& stream, const std::string& string) const;
        };
        
        //! Default pretty formatting settings that come with libjsonata
        class PrettyFormat;
        
    public:
        //! Construct the writer by passing a value and format
        /*! @warning The value and format should outlive the Writer */
        Writer(const Value& value, const Format& format = {});
        
    public:
        //! The value that will be outputted
        const Value& value;
        
        //! The format that is used for outputting
        const Format& format;
    };
    
    //! Pretty settings for Json formatting
    class Writer::PrettyFormat : public Writer::Format
    {
    public:
        //! Write a Json value to stream using pretty formatting
        void write(std::ostream& stream, const Value& value) const override;
    };
    
    //! Stream a json writer with formatting to an output stream
    std::ostream& operator<<(std::ostream& stream, const Writer& writer);
}

#endif
