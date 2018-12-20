//
//  lexer.hpp
//  Jsonata
//
//  Created by Stijn Frishert on 12/20/18.
//

#pragma once

#include <cstddef>

#include <istream>
#include <string_view>

#include "token.hpp"

namespace json
{
    class Lexer
    {
    public:
        Lexer(std::istream& stream);
        
        Token getNextToken();
        
    private:
        void consumeWhitespace();
        Token consumeNumber();
        Token consumeIdentifier();
        Token consumeString();
        std::string consumeUtf32CodePoint();
        
    private:
        std::istream& stream;
        
        std::size_t line = 1;
        std::size_t character = 1;
    };
}
