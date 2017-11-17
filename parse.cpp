//
//  parse.cpp
//  Jsonata
//
//  Copyright © 2015-2016 Dsperados (info@dsperados.com). All rights reserved.
//  Licensed under the BSD 3-clause license.
//

#include <cassert>
#include <codecvt>
#include <ios>
#include <locale>
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
        vector<char> reality(expectation.size());
        long numRead = stream.readsome(reality.data(), static_cast<long>(expectation.size()));
        
        if (expectation == string(reality.data(), static_cast<unsigned long>(numRead)))
            return true;
        
        for (auto i = numRead - 1; i >= 0; --i)
            stream.putback(reality[static_cast<std::size_t>(i)]);
        
        return false;
    }
    
    Value parseNumber(istream& stream)
    {
        bool negative = false;
        string token;
        if (stream.peek() == '-')
        {
            negative = true;
            token += static_cast<char>(stream.get());
        }
        
        while (isdigit(stream.peek()))
           token += static_cast<char>(stream.get());
        
        if (stream.peek() != '.')
            return negative ? Value(stoll(token)) : Value(stoull(token));
        
        token += static_cast<char>(stream.get());
        while (isdigit(stream.peek()))
            token += static_cast<char>(stream.get());
        
        return stold(token);
    }
    
    string parseString(istream& stream)
    {
        assert(stream.peek() == '"');
        stream.ignore(); // "
        
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
                        uint16_t codePoint = 0;
                        stream >> hex >> codePoint;

#ifndef WIN32
					static wstring_convert<codecvt_utf8<char32_t>, char32_t> converter;
					string += converter.to_bytes(codePoint);
#else
					static wstring_convert<codecvt_utf8<wchar_t>, wchar_t> converter;
					string += converter.to_bytes(codePoint);
#endif
                        
                        break;
                }
            } else {
                string += static_cast<char>(c);
            }
            
            if (stream.eof())
                throw runtime_error("Json reached end-of-stream, but expected '\"' first");
        }
        
        return string;
    }
    
    Value parseArray(istream& stream)
    {
        assert(stream.peek() == '[');
        stream.ignore(); // [
        
        Value::Array array;
        
        while (true)
        {
            // Make sure we eject in case of an empty array
            skipWhitespaceAndCheckEof(stream, ']');
            if (stream.peek() == ']')
            {
                stream.ignore();
                return array;
            }
            
            // Append the array
            array.emplace_back(parse(stream));
            
            // Skip possible whitespace
            skipWhitespaceAndCheckEof(stream, ']');
            
            switch (stream.peek())
            {
                case ',':
                    stream.ignore();
                    continue;
                case ']':
                    stream.ignore();
                    return array;
                default:
                    throw runtime_error("Json expected either ',' or ']'");
            }
        }
    }
    
    Value parseObject(istream& stream)
    {
        assert(stream.peek() == '{');
        stream.ignore(); // {
        
        Value::Object object;
        
        while (true)
        {
            // Make sure we eject in case of an empty object
            skipWhitespaceAndCheckEof(stream, '}');
            if (stream.peek() == '}')
            {
                stream.ignore();
                return object;
            }
            
            // Read the key
            if (stream.peek() != '"')
                throw runtime_error("expected a '\"' to start an object key");
            
            const auto key = parseString(stream);
            skipWhitespace(stream);
            if (stream.get() != ':')
                throw runtime_error("Json expected a ':' after an object key");
                        
            // Parse the value
            object.emplace(key, parse(stream));
            
            // Skip possible whitespace
            skipWhitespaceAndCheckEof(stream, '}');
            
            switch (stream.peek())
            {
                case ',':
                    stream.ignore();
                    continue;
                case '}':
                    stream.ignore();
                    return object;
                default:
                    throw runtime_error("Json expected either ',' or '}'");
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
        else if (stream.peek() == '-' || stream.peek() == '.' || isdigit(stream.peek()))
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
