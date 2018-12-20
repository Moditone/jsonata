//
//  lexer.hpp
//  Jsonata
//
//  Created by Stijn Frishert on 12/20/18.
//

#pragma once

#include <cstddef>
#include <istream>
#include <stack>
#include <string_view>

#include "token.hpp"

namespace json
{
    class Lexer
    {
    public:
        Lexer(std::istream& stream);
        
        [[nodiscard]] Token getNextToken();
        
    public:
        //! Do we accept comments, even though they are not part of the specification?
        bool acceptComments = true;
        
    private:
        void consumeWhitespaceAndComments();
        void consumeWhitespace();
        
        [[nodiscard]] Token consumeNumber();
        [[nodiscard]] Token consumeIdentifier();
        [[nodiscard]] Token consumeString();
        [[nodiscard]] std::string consumeUtf32CodePoint();
        
        [[nodiscard]] char peek();
        [[nodiscard]] char peek(std::size_t offset);
        
        [[nodiscard]] char get();
        [[nodiscard]] char getWithoutPositionChange();
        
        void ignore();
        void ignoreLine();
        void ignoreBlockComment();
        
        void rollBack(char c);
        
        [[nodiscard]] Token createToken(Token::Type type, std::string_view lexeme) const;
        [[nodiscard]] Token createToken(Token::Type type, char c) const;
        
    private:
        std::istream& stream;
        
        std::size_t line = 0;
        std::size_t character = 0;
        
        std::stack<char> rollBackStack;
    };
}
