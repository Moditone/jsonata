//
//  token.hpp
//  Jsonata
//
//  Created by Stijn Frishert on 12/20/18.
//

#pragma once

#include <string>
#include <string_view>

namespace json
{
    class Token
    {
    public:
        enum class Type
        {
            LEFT_ACCOLADE,
            RIGHT_ACCOLADE,
            LEFT_SQUARE_BRACKET,
            RIGHT_SQUARE_BRACKET,
            COLON,
            COMMA,
            BOOL_TRUE,
            BOOL_FALSE,
            NUMBER,
            STRING,
            NIL,
            END_OF_FILE,
            UNKNOWN
        };
        
    public:
        Token() : type(Type::END_OF_FILE) { }
        
        Token(Type type, std::string_view lexeme, std::size_t line, std::size_t character) :
            type(type),
            lexeme(lexeme),
            line(line),
            character(character)
        {
            
        }
        
    public:
        Type type = Type::UNKNOWN;
        std::string lexeme;
        
        std::size_t line = 0;
        std::size_t character = 0;
    };
}
