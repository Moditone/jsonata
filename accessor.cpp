//
//  accessor.hpp
//  Jsonata
//
//  Created by Stijn Frishert (info@stijnfrishert.com) on 10/10/2016.
//  Copyright © 2015-2016 Stijn Frishert. All rights reserved.
//  Licensed under the BSD 3-clause license.
//

#include <stdexcept>

#include "value.hpp"

using namespace std;

namespace json
{

// --- Accessor --- //
    
    Value::Accessor::Accessor(const Value::Iterator& iterator) :
        iterator(iterator.asVariant())
    {
        
    }
    
    const string& Value::Accessor::key()
    {
        if (iterator.type() == typeid(Object::iterator))
            return boost::get<Object::iterator>(iterator)->first;
        
        throw runtime_error("Accessor does not point to an object element, yet key() was called on it");
    }
    
    Value& Value::Accessor::value()
    {
        switch (iterator.which())
        {
            case 0: return *boost::get<Array::iterator>(iterator);
            case 1: return boost::get<Object::iterator>(iterator)->second;
            default: assert(false);
        }
    }
    
// --- ConstAccessor --- //
    
    Value::ConstAccessor::ConstAccessor(const Value::ConstIterator& iterator) :
        iterator(iterator.asVariant())
    {
        
    }
    
    const string& Value::ConstAccessor::key()
    {
        if (iterator.type() == typeid(Object::const_iterator))
            return boost::get<Object::const_iterator>(iterator)->first;
        
        throw runtime_error("Accessor does not point to an object element, yet key() was called on it");
    }
    
    const Value& Value::ConstAccessor::value()
    {
        switch (iterator.which())
        {
            case 0: return *boost::get<Array::const_iterator>(iterator);
            case 1: return boost::get<Object::const_iterator>(iterator)->second;
            default: assert(false);
        }
    }
}
