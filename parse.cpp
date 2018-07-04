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

#include "error.hpp"
#include "parse.hpp"

using namespace std;

namespace json
{
    class Parser
    {
    public:
        Value parse(std::istream& stream);
        
    private:
        void skipWhitespace(std::istream& stream);
        void skipWhitespaceAndCheckEof(std::istream& stream, char expectation);
        bool expect(std::istream& stream, const std::string& expectation);
        Value parseNumber(std::istream& stream);
        std::string parseString(std::istream& stream);
        Value parseArray(std::istream& stream);
        Value parseObject(std::istream& stream);
        
        Error createError(const std::string& message);
        
    private:
        std::size_t line = 1;
        std::size_t character = 1;
    };
    
    Error Parser::createError(const std::string& message)
    {
        return Error(line, character, message);
    }
    
    void Parser::skipWhitespace(std::istream& stream)
    {
        while (isspace(stream.peek()))
        {
            if (stream.peek() == '\n')
            {
                line += 1;
                character = 1;
            } else {
                character += 1;
            }
            
            stream.ignore();
        }
    }
    
    void Parser::skipWhitespaceAndCheckEof(std::istream& stream, char expectation)
    {
        skipWhitespace(stream);
    
        if (!stream.good() || stream.eof())
            throw createError("Json reached end-of-stream, but expected " + string(1, expectation) + " first");
    }
    
    bool Parser::expect(std::istream& stream, const std::string& expectation)
    {
        assert(stream.good());
        
        vector<char> reality(expectation.size());
        stream.read(reality.data(), static_cast<long>(expectation.size()));
        long numRead = stream.gcount();
        if (numRead == expectation.size() && expectation == string(reality.data(), static_cast<unsigned long>(numRead)))
        {
            character += static_cast<std::size_t>(numRead);
            return true;
        }
        
        if (stream.eof())
            stream.clear();
        
        for (auto i = numRead - 1; i >= 0; --i)
            stream.putback(reality[static_cast<std::size_t>(i)]);
        
        assert(stream.good());
        
        return false;
    }
    
    Value Parser::parseNumber(std::istream& stream)
    {
        bool negative = false;
        string token;
        if (stream.peek() == '-')
        {
            negative = true;
            token += static_cast<char>(stream.get());
            character += 1;
        }
        
        while (isdigit(stream.peek()))
        {
            token += static_cast<char>(stream.get());
            character += 1;
        }
        
        if (stream.peek() != '.')
            return negative ? Value(std::stoll(token)) : Value(std::stoull(token));
        
        token += static_cast<char>(stream.get());
        character += 1;
        while (isdigit(stream.peek()))
        {
            token += static_cast<char>(stream.get());
            character += 1;
        }
        
        return std::stold(token);
    }
    
    std::string Parser::parseString(std::istream& stream)
    {
        assert(stream.peek() == '"');
        stream.ignore(); // "
        character += 1;
        
        std::string string;
        
        for (auto c = stream.get(); c != '"'; c = stream.get())
        {
            if (c == '\\')
            {
                character += 1;
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
                        character += bytes.size();
                        
                        break;
                }
            } else {
                string += static_cast<char>(c);
                character += 1;
            }
            
            if (stream.eof())
                throw createError("Json reached end-of-stream, but expected '\"' first");
        }
        
        return string;
    }
    
    Value Parser::parseArray(std::istream& stream)
    {
        assert(stream.peek() == '[');
        stream.ignore(); // [
        character += 1;
        
        Value::Array array;
        
        while (true)
        {
            // Make sure we eject in case of an empty array
            skipWhitespaceAndCheckEof(stream, ']');
            if (stream.peek() == ']')
            {
                stream.ignore();
                character += 1;
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
                    character += 1;
                    continue;
                case ']':
                    stream.ignore();
                    character += 1;
                    return array;
                default:
                    throw createError("Json expected either ',' or ']'");
            }
        }
    }
    
    Value Parser::parseObject(std::istream& stream)
    {
        assert(stream.peek() == '{');
        stream.ignore(); // {
        character += 1;
        
        Value::Object object;
        
        while (true)
        {
            // Make sure we eject in case of an empty object
            skipWhitespaceAndCheckEof(stream, '}');
            if (stream.peek() == '}')
            {
                stream.ignore();
                character += 1;
                return object;
            }
            
            // Read the key
            if (stream.peek() != '"')
                throw createError("expected a '\"' to start an object key");
            
            const auto key = parseString(stream);
            skipWhitespace(stream);
            if (stream.get() != ':')
                throw createError("Json expected a ':' after an object key");
            character += 1;
                        
            // Parse the value
            object.emplace(key, parse(stream));
            
            // Skip possible whitespace
            skipWhitespaceAndCheckEof(stream, '}');
            
            switch (stream.peek())
            {
                case ',':
                    stream.ignore();
                    character += 1;
                    continue;
                case '}':
                    stream.ignore();
                    character += 1;
                    return object;
                default:
                    throw createError("Json expected either ',' or '}'");
            }
        }
    }
    
    Value Parser::parse(std::istream& stream)
    {
        // Skip possible whitespace
        skipWhitespace(stream);
        if (stream.eof())
            throw createError("Json reached end-of-stream with an empty stream");
        
        const auto peek = stream.peek();
        if (expect(stream, "null"))
            return Value::null;
        else if (expect(stream, "false"))
            return false;
        else if (expect(stream, "true"))
            return true;
        else if (peek == '-' || stream.peek() == '.' || isdigit(stream.peek()))
            return parseNumber(stream);
        
        switch (stream.peek())
        {
            case '\"': return parseString(stream);
            case '[': return parseArray(stream);
            case '{': return parseObject(stream);
        }
        
        throw createError("Json found unsupported character '" + std::string(1, stream.peek()) + "'");
    }

	Value parse(std::istream& stream)
	{
        return Parser().parse(stream);
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
