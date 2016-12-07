//
//  parse.hpp
//  Jsonata
//
//  Copyright © 2015-2016 Dsperados (info@dsperados.com). All rights reserved.
//  Licensed under the BSD 3-clause license.
//

#ifndef JSON_PARSE_HPP
#define JSON_PARSE_HPP

#include <istream>
#include <string>

#include "value.hpp"

namespace json
{
	//! Parse a Json value from stream
	/*! @throw std::runtime_error in case of parsing errors */
	Value parse(std::istream& stream);
    
    //! Parse a Json value from text
    /*! @throw std::runtime_error in case of parsing errors */
    Value parse(const std::string& text);
    
    //! Parse a json value from stream
    std::istream& operator>>(std::istream& stream, Value& value);
}

#endif
