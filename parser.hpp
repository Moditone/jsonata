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
    class Lexer;
    class Token;
    
    class Parser
    {
    public:
        Parser(Lexer& lexer);
        
        [[nodiscard]] Value parse();
        
    public:
        //! Do we accept a comma after the last entry of an object or array?
        /*! Technically this is not correct Json, but happens often with copy/paste json
            and there's no harm in accepting this. */
        bool acceptCommaAfterLastEntry = true;
        
    private:
        [[nodiscard]] Value parse(const Token& token);
        [[nodiscard]] Value parseObject();
        [[nodiscard]] Value parseArray();
        [[nodiscard]] Value parseNumber(std::string_view lexeme);
        
    private:
        Lexer& lexer;
    };
}
