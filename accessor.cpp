//
//  accessor.hpp
//  Jsonata
//
//  Copyright © 2015-2016 Dsperados (info@dsperados.com). All rights reserved.
//  Licensed under the BSD 3-clause license.
//

#include <stdexcept>

#include "value.hpp"

using namespace std;

namespace json
{

// --- Accessor --- //
    
    Value::Accessor::Accessor(Array::iterator iterator) :
        toArray(true),
        itArray(iterator)
    {
        
    }
    
    Value::Accessor::Accessor(Object::iterator iterator) :
        toArray(false),
        itObject(iterator)
    {
        
    }
    
    const string& Value::Accessor::key()
    {
        if (toArray)
            throw runtime_error("Accessor does not point to an object element, yet key() was called on it");
        
        return itObject->first;
    }
    
    Value& Value::Accessor::value()
    {
        return toArray ? *itArray : itObject->second;
    }
    
// --- ConstAccessor --- //
    
    Value::ConstAccessor::ConstAccessor(Array::const_iterator iterator) :
        toArray(true),
        itArray(iterator)
    {
        
    }
    
    Value::ConstAccessor::ConstAccessor(Object::const_iterator iterator) :
        toArray(false),
        itObject(iterator)
    {
        
    }
    
    const string& Value::ConstAccessor::key()
    {
        if (toArray)
            throw runtime_error("ConstAccessor does not point to an object element, yet key() was called on it");
        
        return itObject->first;
    }
    
    const Value& Value::ConstAccessor::value()
    {
        return toArray ? *itArray : itObject->second;
    }
}
