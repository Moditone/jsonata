//
//  token.hpp
//  Jsonata
//
//  Created by Stijn Frishert on 12/20/18.
//

#pragma once

#include <string>

namespace json
{
    struct Token
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
        } type = Type::END_OF_FILE;
        
    public:
        Token() : type(Type::END_OF_FILE) { }
        
        Token(Type type, const std::string& lexeme) :
            type(type),
            lexeme(lexeme)
        {
            
        }
        
    public:
        std::string lexeme;
    };
}
