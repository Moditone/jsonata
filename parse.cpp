//
//  parse.cpp
//  Jsonata
//
//  Created by Stijn Frishert (info@stijnfrishert.com) on 10/10/2016.
//  Copyright © 2015-2016 Stijn Frishert. All rights reserved.
//  Licensed under the BSD 3-clause license.
//

#include <cassert>
#include <codecvt>
#include <sstream>
#include <stdexcept>
#include <string>

#include "parse.hpp"

using namespace std;

namespace json
{
	void skipWhitespace(istream& stream)
    {
        while (isspace(stream.peek()))
            stream.ignore();
    }
    
    void skipWhitespaceAndCheckEof(istream& stream, char expectation)
    {
        skipWhitespace(stream);
               
        if (!stream.good() || stream.eof())
            throw runtime_error("Json reached end-of-stream, but expected " + string(1, expectation) + " first");
    }
    
    bool expect(istream& stream, const string& expectation)
    {
        char reality[expectation.size()];
        auto numRead = stream.readsome(reality, expectation.size());
        
        if (expectation == string(reality, numRead))
            return true;
        
        stream.seekg(-numRead, ios_base::seekdir::cur);
        return false;
    }
    
    Value parseNumber(istream& stream)
    {
        bool negative = false;
        string token;
        if (stream.peek() == '-')
        {
            negative = true;
            token += stream.get();
        }
        
        while (isdigit(stream.peek()))
           token += stream.get();
        
        if (stream.peek() != '.')
            return negative ? Value(stoll(token)) : Value(stoull(token));
        
        token += stream.get();
        while (isdigit(stream.peek()))
            token += stream.get();
        
        return stold(token);
    }
    
    string parseString(istream& stream)
    {
        assert(stream.get() == '"');
        
        string string;
        
        for (auto c = stream.get(); c != '"'; c = stream.get())
        {
            if (c == '\\')
            {
                switch (stream.get())
                {
                    case '\"': string += '\"'; break;
                    case '\\': string += '\\'; break;
                    case '/': string += '/'; break;
                    case 'b': string += '\b'; break;
                    case 'f': string += '\f'; break;
                    case 'n': string += '\n'; break;
                    case 'r': string += '\r'; break;
                    case 't': string += '\t'; break;
                    case 'u':
                        uint16_t codePoint;
                        stream >> hex >> codePoint;
                        
                        static wstring_convert<codecvt_utf8<char32_t>, char32_t> converter;
                        string += converter.to_bytes(codePoint);
                        
                        break;
                }
            } else {
                string += c;
            }
            
            if (stream.eof())
                throw runtime_error("Json reached end-of-stream, but expected '\"' first");
        }
        
        return string;
    }
    
    Value parseArray(istream& stream)
    {
        assert(stream.get() == '[');
        
        auto value = Value::array;
        
        while (true)
        {
            // Make sure we eject in case of an empty array
            skipWhitespaceAndCheckEof(stream, ']');
            if (stream.peek() == ']')
            {
                stream.ignore();
                return value;
            }
            
            // Append the array
            value.append(parse(stream));
            
            // Skip possible whitespace
            skipWhitespaceAndCheckEof(stream, ']');
            
            switch (stream.get())
            {
                case ',': continue;
                case ']': return value;
                default: throw runtime_error("Json expected either ',' or ']'");
            }
        }
    }
    
    Value parseObject(istream& stream)
    {
        assert(stream.get() == '{');
        
        auto value = Value::object;
        
        while (true)
        {
            // Make sure we eject in case of an empty object
            skipWhitespaceAndCheckEof(stream, '}');
            if (stream.peek() == '}')
            {
                stream.ignore();
                return value;
            }
            
            // Read the key
            const auto key = parseString(stream);
            if (stream.get() != ':')
                throw runtime_error("Json expected a ':' after an object key");
            
            // Parse the value
            value[key] = parse(stream);
            
            // Skip possible whitespace
            skipWhitespaceAndCheckEof(stream, '}');
            
            switch (stream.get())
            {
                case ',': continue;
                case '}': return value;
                default: throw runtime_error("Json expected either ',' or '}'");
            }
        }
    }

	Value parse(istream& stream)
	{
		// Skip possible whitespace
        skipWhitespace(stream);
        if (stream.eof())
            throw runtime_error("Json reached end-of-stream with an empty stream");
        
        if (expect(stream, "null"))
            return Value::null;
        else if (expect(stream, "false"))
            return false;
        else if (expect(stream, "true"))
            return true;
        else if (stream.peek() == '-' || isdigit(stream.peek()))
            return parseNumber(stream);
        
        switch (stream.peek())
        {
            case '\"': return parseString(stream);
            case '[': return parseArray(stream);
            case '{': return parseObject(stream);
        }
        
        throw runtime_error("Json found unsupported character '" + string(1, stream.peek()) + "'");
	}
    
    Value parse(const string& text)
    {
        istringstream stream(text);
        return parse(stream);
    }
    
    istream& operator>>(istream& stream, Value& value)
    {
        value = parse(stream);
        return stream;
    }
}
