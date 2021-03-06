//
//  lexer.cpp
//  Jsonata
//
//  Created by Stijn Frishert on 12/20/18.
//

#include <array>
#include <cassert>
#include <codecvt>
#include <cstdint>
#include <locale>
#include <vector>

#include "lexer.hpp"

namespace json
{
    Lexer::Lexer(std::istream& stream) :
        stream(stream)
    {
        
    }
    
    Token Lexer::getNextToken()
    {
        consumeWhitespaceAndComments();
        
        if (stream.eof())
            return createToken(Token::Type::END_OF_FILE, "");
        else if (!stream.good())
            return createToken(Token::Type::UNKNOWN, "");
        
        const auto c = peek();
        if (stream.eof())
            return createToken(Token::Type::END_OF_FILE, "");
        
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
        
        if (::isdigit(c))
            return consumeNumber();
        else
            return consumeIdentifier();
    }
    
    void Lexer::consumeWhitespaceAndComments()
    {
        consumeWhitespace();
        
        if (!acceptComments)
            return;
        
        switch (peek())
        {
            case '#':
                ignoreLine();
                return consumeWhitespaceAndComments();
            case '/':
                switch (peek(1))
                {
                    case '/':
                        ignoreLine();
                        consumeWhitespaceAndComments();
                        return;
                    case '*':
                        ignoreBlockComment();
                        consumeWhitespaceAndComments();
                        return;
                }
        }
    }
    
    void Lexer::consumeWhitespace()
    {
        while (true)
        {
            if (const auto p = peek(); stream.eof() || !::isspace(p))
                break;
            
            ignore();
        }
    }
    
    Token Lexer::consumeNumber()
    {
        assert(stream.good());
        
        std::string lexeme;
        if (peek() == '-')
            lexeme += get();
        
        lexeme += consumeDigitString();
        
        if (const auto p = peek(); stream.good() && p == '.')
        {
            lexeme += get();
            lexeme += consumeDigitString();
        }
        
        if (const auto p = peek(); stream.good() && (p == 'e' || p == 'E'))
        {
            lexeme += get();
            
            if (const auto p = peek(); stream.good() && (p == '+' || p == '-'))
                lexeme += get();
            
            lexeme += consumeDigitString();
        }
        
        return createToken(Token::Type::NUMBER, lexeme);
    }
    
    std::string Lexer::consumeDigitString()
    {
        std::string string;
        
        while (true)
        {
            if (const auto p = peek(); !stream.good() || !::isdigit(p))
                break;
            
            string += get();
        }
        
        return string;
    }
    
    Token Lexer::consumeIdentifier()
    {
        std::string lexeme;
        while (true)
        {
            if (const auto p = peek(); !stream.good() || !::isalpha(p))
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
                        lexeme += c;
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
        if (!rollBackStack.empty())
            return rollBackStack.top();
        else
            return static_cast<char>(stream.peek());
    }
    
    char Lexer::peek(std::size_t offset)
    {
        std::vector<char> history(offset, 0);
        for (auto& c : history)
            c = getWithoutPositionChange();
        
        auto p = peek();
        
        for (auto it = history.rbegin(); it != history.rend(); it++)
            rollBack(*it);
        
        return p;
    }
    
    char Lexer::get()
    {
        const auto c = getWithoutPositionChange();
        
        if (c == '\n')
        {
            line += 1;
            character = 0;
        } else {
            character += 1;
        }
        
        return c;
    }
    
    char Lexer::getWithoutPositionChange()
    {
        if (rollBackStack.empty())
        {
            return static_cast<char>(stream.get());
        }
        else
        {
            const auto c = rollBackStack.top();
            rollBackStack.pop();
            return c;
        }
    }
    
    void Lexer::ignore()
    {
        // Call get, because we don't actually want to ignore but
        // increment the line or character in case of certain values
        [[maybe_unused]] const auto c = get();
    }
    
    void Lexer::ignoreLine()
    {
        while (get() != '\n')
        {
            if (!stream.good())
                return;
        }
    }
    
    void Lexer::ignoreBlockComment()
    {
        while (true)
        {
            if (get() == '*' && peek() == '/')
            {
                ignore();
                return;
            }
            
            if (!stream.good())
                return;
        }
    }
    
    void Lexer::rollBack(char c)
    {
        rollBackStack.push(c);
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
