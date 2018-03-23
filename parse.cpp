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
    struct State
    {
        std::size_t line = 1;
        std::size_t character = 1;
    };
    
	void skipWhitespace(std::istream& stream, State& state)
    {
        while (isspace(stream.peek()))
        {
            if (stream.peek() == '\n')
            {
                state.line += 1;
                state.character = 1;
            } else {
                state.character += 1;
            }
            
            stream.ignore();
        }
    }
    
    void skipWhitespaceAndCheckEof(std::istream& stream, char expectation, State& state)
    {
        skipWhitespace(stream, state);
    
        if (!stream.good() || stream.eof())
            throw runtime_error("Json reached end-of-stream, but expected " + string(1, expectation) + " first");
    }
    
    bool expect(std::istream& stream, const std::string& expectation, State& state)
    {
        vector<char> reality(expectation.size());
        long numRead = stream.readsome(reality.data(), static_cast<long>(expectation.size()));
        
        if (expectation == string(reality.data(), static_cast<unsigned long>(numRead)))
        {
            state.character += static_cast<std::size_t>(numRead);
            return true;
        }
        
        for (auto i = numRead - 1; i >= 0; --i)
            stream.putback(reality[static_cast<std::size_t>(i)]);
        
        return false;
    }
    
    Value parse(std::istream& stream, State& state);
    
    Value parseNumber(std::istream& stream, State& state)
    {
        bool negative = false;
        string token;
        if (stream.peek() == '-')
        {
            negative = true;
            token += static_cast<char>(stream.get());
            state.character += 1;
        }
        
        while (isdigit(stream.peek()))
        {
           token += static_cast<char>(stream.get());
            state.character += 1;
        }
        
        if (stream.peek() != '.')
            return negative ? Value(std::stoll(token)) : Value(std::stoull(token));
        
        token += static_cast<char>(stream.get());
        state.character += 1;
        while (isdigit(stream.peek()))
        {
            token += static_cast<char>(stream.get());
            state.character += 1;
        }
        
        return std::stold(token);
    }
    
    std::string parseString(std::istream& stream, State& state)
    {
        assert(stream.peek() == '"');
        stream.ignore(); // "
        state.character += 1;
        
        std::string string;
        
        for (auto c = stream.get(); c != '"'; c = stream.get())
        {
            if (c == '\\')
            {
                state.character += 1;
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

                        std::string bytes;
#ifndef WIN32
                        static std::wstring_convert<codecvt_utf8<char32_t>, char32_t> converter;
                        bytes = converter.to_bytes(codePoint);
#else
                        static std::wstring_convert<codecvt_utf8<wchar_t>, wchar_t> converter;
                        bytes = converter.to_bytes(codePoint);
                        string += bytes;
#endif
                        state.character += bytes.size();
                        
                        break;
                }
            } else {
                string += static_cast<char>(c);
                state.character += 1;
            }
            
            if (stream.eof())
                throw runtime_error("Json reached end-of-stream, but expected '\"' first");
        }
        
        return string;
    }
    
    Value parseArray(std::istream& stream, State& state)
    {
        assert(stream.peek() == '[');
        stream.ignore(); // [
        state.character += 1;
        
        Value::Array array;
        
        while (true)
        {
            // Make sure we eject in case of an empty array
            skipWhitespaceAndCheckEof(stream, ']', state);
            if (stream.peek() == ']')
            {
                stream.ignore();
                state.character += 1;
                return array;
            }
            
            // Append the array
            array.emplace_back(parse(stream, state));
            
            // Skip possible whitespace
            skipWhitespaceAndCheckEof(stream, ']', state);
            
            switch (stream.peek())
            {
                case ',':
                    stream.ignore();
                    state.character += 1;
                    continue;
                case ']':
                    stream.ignore();
                    state.character += 1;
                    return array;
                default:
                    throw runtime_error("Json expected either ',' or ']'");
            }
        }
    }
    
    Value parseObject(std::istream& stream, State& state)
    {
        assert(stream.peek() == '{');
        stream.ignore(); // {
        state.character += 1;
        
        Value::Object object;
        
        while (true)
        {
            // Make sure we eject in case of an empty object
            skipWhitespaceAndCheckEof(stream, '}', state);
            if (stream.peek() == '}')
            {
                stream.ignore();
                state.character += 1;
                return object;
            }
            
            // Read the key
            if (stream.peek() != '"')
                throw runtime_error("expected a '\"' to start an object key");
            
            const auto key = parseString(stream, state);
            skipWhitespace(stream, state);
            if (stream.get() != ':')
                throw runtime_error("Json expected a ':' after an object key");
            state.character += 1;
                        
            // Parse the value
            object.emplace(key, parse(stream, state));
            
            // Skip possible whitespace
            skipWhitespaceAndCheckEof(stream, '}', state);
            
            switch (stream.peek())
            {
                case ',':
                    stream.ignore();
                    state.character += 1;
                    continue;
                case '}':
                    stream.ignore();
                    state.character += 1;
                    return object;
                default:
                    throw runtime_error("Json expected either ',' or '}'");
            }
        }
    }
    
    Value parse(std::istream& stream, State& state)
    {
        // Skip possible whitespace
        skipWhitespace(stream, state);
        if (stream.eof())
            throw runtime_error("Json reached end-of-stream with an empty stream");
        
        if (expect(stream, "null", state))
            return Value::null;
        else if (expect(stream, "false", state))
            return false;
        else if (expect(stream, "true", state))
            return true;
        else if (stream.peek() == '-' || stream.peek() == '.' || isdigit(stream.peek()))
            return parseNumber(stream, state);
        
        switch (stream.peek())
        {
            case '\"': return parseString(stream, state);
            case '[': return parseArray(stream, state);
            case '{': return parseObject(stream, state);
        }
        
        throw runtime_error("Json found unsupported character '" + string(1, stream.peek()) + "'");
    }

	Value parse(std::istream& stream)
	{
        State state;
        state.line = 1;
        state.character = 1;
        
        return parse(stream, state);
	}
    
    Value parse(const std::string& text)
    {
        istringstream stream(text);
        return parse(stream);
    }
    
    istream& operator>>(std::istream& stream, Value& value)
    {
        value = parse(stream);
        return stream;
    }
}
