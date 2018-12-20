//
//  parse.cpp
//  Jsonata
//
//  Copyright © 2015-2016 Dsperados (info@dsperados.com). All rights reserved.
//  Licensed under the BSD 3-clause license.
//

#include <cassert>
#include <codecvt>
#include <ios>
#include <locale>
#include <sstream>
#include <stdexcept>
#include <string>

#include "error.hpp"
#include "parse.hpp"
#include "parser.hpp"
#include "lexer.hpp"

using namespace std;

namespace json
{
	Value parse(std::istream& stream)
	{
        Lexer lexer(stream);
        Parser parser(lexer);
        return parser.parse();
	}
    
    Value parse(const std::string& text)
    {
        istringstream stream(text);
        return parse(stream);
    }
    
    istream& operator>>(std::istream& stream, Value& value)
    {
        value = parse(stream);
        return stream;
    }
}
