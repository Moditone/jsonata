//
//  parser.hpp
//  Jsonata
//
//  Created by Stijn Frishert on 12/20/18.
//

#pragma once

#include <string>

#include "value.hpp"

namespace json
{
    class Parser
    {
    public:
        Parser(Lexer& lexer);
        
        Value parse();
        
    private:
        Value parse(const Token& token);
        Value parseObject();
        Value parseArray();
        Value parseNumber(std::string_view lexeme);
        
    private:
        Lexer& lexer;
    };
}
