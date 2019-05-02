//
//  parser.cpp
//  Jsonata
//
//  Created by Stijn Frishert on 12/20/18.
//

#include <algorithm>

#include "lexer.hpp"
#include "parser.hpp"

namespace json
{
    Parser::Parser(Lexer& lexer) :
        lexer(lexer)
    {
        
    }
    
    Value Parser::parse()
    {
        return parse(lexer.getNextToken());
    }
    
    Value Parser::parse(const Token& token)
    {
        switch (token.type)
        {
            case Token::Type::LEFT_ACCOLADE: return parseObject();
            case Token::Type::LEFT_SQUARE_BRACKET: return parseArray();
            case Token::Type::STRING: return token.lexeme;
            case Token::Type::NUMBER: return parseNumber(token.lexeme);
            case Token::Type::BOOL_TRUE: return true;
            case Token::Type::BOOL_FALSE: return false;
            case Token::Type::NIL: return json::Value::null;
            default: throw std::runtime_error("Unexpected token");
        }
    }
    
    Value Parser::parseObject()
    {
        auto object = json::Value::emptyObject;
        
        auto token = lexer.getNextToken();
        if (token.type == Token::Type::RIGHT_ACCOLADE)
            return object;
        
        while (true)
        {
            if (token.type != Token::Type::STRING)
                throw std::runtime_error("Unexpected token");
            
            const auto key = token.lexeme;
            if (lexer.getNextToken().type != Token::Type::COLON)
                throw std::runtime_error("Expected : after an object key");
            
            object.insert(key, parse());
            
            token = lexer.getNextToken();
            if (token.type == Token::Type::RIGHT_ACCOLADE)
                break;
            else if (token.type != Token::Type::COMMA)
                throw std::runtime_error("Object fields must be seperated by ,");
            
            token = lexer.getNextToken();
            
            if (acceptCommaAfterLastEntry && token.type == Token::Type::RIGHT_ACCOLADE)
                break;
        }
        
        return object;
    }
    
    Value Parser::parseArray()
    {
        auto array = json::Value::emptyArray;
        
        auto token = lexer.getNextToken();
        if (token.type == Token::Type::RIGHT_SQUARE_BRACKET)
            return array;
        
        while (true)
        {
            array.append(parse(token));
            
            token = lexer.getNextToken();
            if (token.type == Token::Type::RIGHT_SQUARE_BRACKET)
                break;
            else if (token.type != Token::Type::COMMA)
                throw std::runtime_error("Array fields must be seperated by ,");
            
            token = lexer.getNextToken();
            
            if (acceptCommaAfterLastEntry && token.type == Token::Type::RIGHT_SQUARE_BRACKET)
                break;
        }
        
        return array;
    }
    
    Value Parser::parseNumber(std::string_view lexeme)
    {
        if (std::any_of(lexeme.begin(), lexeme.end(), [](auto c){ return c == '.'; }))
            return std::stold(std::string(lexeme));
        else
            return std::stoll(std::string(lexeme));
    }
}
