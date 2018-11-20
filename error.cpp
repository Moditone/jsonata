//
//  error.cpp
//  Jsonata
//
//  Created by Stijn Frishert on 6/21/18.
//

#include "error.hpp"

namespace json
{
    Error::Error(std::size_t line, std::size_t character, std::string_view message) :
        std::runtime_error(std::to_string(line) + ":" + std::to_string(character) + " " + std::string(message)),
        line(line),
        character(character),
        message(message)
    {
        
    }
    
    std::size_t Error::getLine() const
    {
        return line;
    }
    
    std::size_t Error::getCharacter() const
    {
        return character;
    }
    
    const std::string& Error::getMessage() const
    {
        return message;
    }
}
