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
        while (std::istream::traits_type::not_eof(peek()))
        {
            consumeWhitespace();
            
            const auto c = peek();
            switch (c)
            {
                case '{': return createToken(Token::Type::LEFT_ACCOLADE, get());
                case '}': return createToken(Token::Type::RIGHT_ACCOLADE, get());
                case '[': return createToken(Token::Type::LEFT_SQUARE_BRACKET, get());
                case ']': return createToken(Token::Type::RIGHT_SQUARE_BRACKET, get());
                case ':': return createToken(Token::Type::COLON, get());
                case ',': return createToken(Token::Type::COMMA, get());
                case '\"': return consumeString();
                case '-': return consumeNumber();
                default: break;
            }
            
            if (std::isdigit(c))
                return consumeNumber();
            else
                return consumeIdentifier();
        }
        
        return createToken(Token::Type::END_OF_FILE, "");
    }
    
    void Lexer::consumeWhitespace()
    {
        while (true)
        {
            const auto p = peek();
            if (stream.eof())
                break;
            
            if (!std::isspace(p))
                break;
            
            ignore();
        }
    }
    
    Token Lexer::consumeNumber()
    {
        assert(std::istream::traits_type::not_eof(peek()));
        
        std::string lexeme;
        if (peek() == '-')
            lexeme += get();
        
        while (std::istream::traits_type::not_eof(peek()) && std::isdigit(peek()))
        {
            lexeme += get();
        }
        
        if (std::istream::traits_type::not_eof(peek()) && peek() == '.')
        {
            lexeme += get();
            
            while (std::istream::traits_type::not_eof(peek()) && std::isdigit(peek()))
            {
                lexeme += get();
            }
        }
        
        if (std::istream::traits_type::not_eof(peek()) && (peek() == 'e' || peek() == 'E'))
        {
            lexeme += get();
            
            if (std::istream::traits_type::not_eof(peek()) && (peek() == '+' || peek() == '-'))
                lexeme += get();
            
            while (std::istream::traits_type::not_eof(peek()) && std::isdigit(peek()))
            {
                lexeme += get();
            }
        }
        
        return createToken(Token::Type::NUMBER, lexeme);
    }
    
    Token Lexer::consumeIdentifier()
    {
        std::string lexeme;
        while (true)
        {
            const auto p = peek();
            if (!std::istream::traits_type::not_eof(p) || !std::isalpha(static_cast<char>(p)))
                break;
            
            lexeme += get();
        
            if (lexeme == "true")
                return createToken(Token::Type::BOOL_TRUE, lexeme);
            else if (lexeme == "false")
                return createToken(Token::Type::BOOL_FALSE, lexeme);
            else if (lexeme == "null")
                return createToken(Token::Type::NIL, lexeme);
        }
        
        return createToken(Token::Type::UNKNOWN, lexeme);
    }
    
    Token Lexer::consumeString()
    {
        assert(peek() == '"');
        ignore();
        
        std::string lexeme;
        while (true)
        {
            const auto p = peek();
            if (!stream.good())
                break;
            
            if (p == '\"')
            {
                ignore();
                break;
            }
            else if (p == '\\')
            {
                ignore();
                const auto c = get();
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
            
            lexeme += get();
        }
        
        return createToken(Token::Type::STRING, lexeme);
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
    
    char Lexer::peek()
    {
        return static_cast<char>(stream.peek());
    }
    
    char Lexer::get()
    {
        auto c = static_cast<char>(stream.get());
        
        if (c == '\n')
        {
            line += 1;
            character = 0;
        } else {
            character += 1;
        }
        
        return c;
    }
    
    void Lexer::ignore()
    {
        // Call get, because we don't actually want to ignore but
        // increment the line or character in case of certain values
        [[maybe_unused]] const auto c = get();
    }
    
    Token Lexer::createToken(Token::Type type, std::string_view lexeme) const
    {
        return {type, lexeme, line, character - lexeme.size()};
    }
    
    Token Lexer::createToken(Token::Type type, char c) const
    {
        return createToken(type, std::string(1, c));
    }
}
