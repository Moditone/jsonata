//
//  error.hpp
//  Jsonata
//
//  Created by Stijn Frishert on 6/21/18.
//

#ifndef JSON_ERROR_HPP
#define JSON_ERROR_HPP

#include <cstddef>
#include <stdexcept>
#include <string>
#include <string_view>

namespace json
{
    class Error :
        public std::runtime_error
    {
    public:
        Error(std::size_t line, std::size_t character, std::string_view message);
        
        std::size_t getLine() const;
        std::size_t getCharacter() const;
        const std::string& getMessage() const;
        
    private:
        std::size_t line = 0;
        std::size_t character = 0;
        std::string message;
    };
}

#endif

