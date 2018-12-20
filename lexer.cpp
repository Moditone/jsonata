//
//  lexer.cpp
//  Jsonata
//
//  Created by Stijn Frishert on 12/20/18.
//

#include <array>
#include <codecvt>
#include <cstdint>
#include <locale>

#include "lexer.hpp"

namespace json
{
    Lexer::Lexer(std::istream& stream) :
        stream(stream)
    {
        
    }
    
    Token Lexer::getNextToken()
    {
        while (std::istream::traits_type::not_eof(stream.peek()))
        {
            consumeWhitespace();
            
            const auto c = stream.peek();
            switch (c)
            {
                case '{': return {Token::Type::LEFT_ACCOLADE, std::string(1, stream.get())};
                case '}': return {Token::Type::RIGHT_ACCOLADE, std::string(1, stream.get())};
                case '[': return {Token::Type::LEFT_SQUARE_BRACKET, std::string(1, stream.get())};
                case ']': return {Token::Type::RIGHT_SQUARE_BRACKET, std::string(1, stream.get())};
                case ':': return {Token::Type::COLON, std::string(1, stream.get())};
                case ',': return {Token::Type::COMMA, std::string(1, stream.get())};
                case '\"': return consumeString();
                case '-': return consumeNumber();
                default: break;
            }
            
            if (std::isdigit(c))
                return consumeNumber();
            else
                return consumeIdentifier();
        }
        
        return {Token::Type::END_OF_FILE, ""};
    }
    
    void Lexer::consumeWhitespace()
    {
        while (std::istream::traits_type::not_eof(stream.peek()) && std::isspace(stream.peek()))
        {
            stream.ignore();
        }
    }
    
    Token Lexer::consumeNumber()
    {
        assert(std::istream::traits_type::not_eof(stream.peek()));
        
        std::string lexeme;
        if (stream.peek() == '-')
            lexeme += static_cast<char>(stream.get());
        
        while (std::istream::traits_type::not_eof(stream.peek()) && std::isdigit(stream.peek()))
        {
            lexeme += static_cast<char>(stream.get());
        }
        
        if (std::istream::traits_type::not_eof(stream.peek()) && stream.peek() == '.')
        {
            lexeme += static_cast<char>(stream.get());
            
            while (std::istream::traits_type::not_eof(stream.peek()) && std::isdigit(stream.peek()))
            {
                lexeme += static_cast<char>(stream.get());
            }
        }
        
        if (std::istream::traits_type::not_eof(stream.peek()) && (stream.peek() == 'e' || stream.peek() == 'E'))
        {
            lexeme += static_cast<char>(stream.get());
            
            if (std::istream::traits_type::not_eof(stream.peek()) && (stream.peek() == '+' || stream.peek() == '-'))
                lexeme += static_cast<char>(stream.get());
            
            while (std::istream::traits_type::not_eof(stream.peek()) && std::isdigit(stream.peek()))
            {
                lexeme += static_cast<char>(stream.get());
            }
        }
        
        return {Token::Type::NUMBER, lexeme};
    }
    
    Token Lexer::consumeIdentifier()
    {
        std::string lexeme;
        while (true)
        {
            const auto peek = stream.peek();
            if (std::istream::traits_type::not_eof(peek) || !std::isalpha(static_cast<char>(peek)))
                break;
            
            lexeme += static_cast<char>(stream.get());
        
            if (lexeme == "true")
                return {Token::Type::BOOL_TRUE, lexeme};
            else if (lexeme == "false")
                return {Token::Type::BOOL_FALSE, lexeme};
            else if (lexeme == "null")
                return {Token::Type::NIL, lexeme};
        }
        
        return {Token::Type::UNKNOWN, lexeme};
    }
    
    Token Lexer::consumeString()
    {
        assert(stream.peek() == '"');
        stream.ignore();
        
        std::string lexeme;
        while (true)
        {
            const auto p = stream.peek();
            if (!stream.good())
                break;
            
            if (p == '\"')
            {
                stream.ignore();
                break;
            }
            else if (p == '\\')
            {
                stream.ignore();
                const auto c = stream.get();
                if (!stream.good())
                    break;
                
                switch (c)
                {
                    case '\"': lexeme += '\"'; break;
                    case '\\': lexeme += '\\'; break;
                    case '/': lexeme += '/'; break;
                    case 'b': lexeme += '\b'; break;
                    case 'f': lexeme += '\f'; break;
                    case 'n': lexeme += '\n'; break;
                    case 'r': lexeme += '\r'; break;
                    case 't': lexeme += '\t'; break;
                    case 'u': lexeme += consumeUtf32CodePoint(); break;
                    default:
                        lexeme += '\\';
                        lexeme += static_cast<char>(c);
                        break;
                }
                
                continue;
            }
            
            lexeme += static_cast<char>(stream.get());
        }
        
        return {Token::Type::STRING, lexeme};
    }
    
    std::string Lexer::consumeUtf32CodePoint()
    {
        std::uint32_t codePoint = 0;
        stream >> std::hex >> codePoint;
        
#ifndef WIN32
        static std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
#else
        static std::wstring_convert<std::codecvt_utf8<std::uint32_t>, std::uint32_t> converter;
#endif
        
        return converter.to_bytes(codePoint);
    }
}
