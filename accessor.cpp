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
    
    // Function calling explicit destructor, circumventing a bug in Clang
    // http://stackoverflow.com/questions/42646680/how-do-i-explicitly-call-the-destructor-of-std-iterators
    template<class T>
    void destroy_at(T* p)
    {
        p->~T();
    }
    
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
    
    Value::Accessor::~Accessor()
    {
        if (toArray)
            destroy_at(&itArray);
        else
            destroy_at(&itObject);
    }
    
    const string& Value::Accessor::key()
    {
        if (toArray)
            throw runtime_error("Accessor does not point to an object element, yet key() was called on it");
        
        return itObject->first;
    }
    
    Value& Value::Accessor::value()
    {
        return toArray ? *itArray : *itObject->second;
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
    
    Value::ConstAccessor::~ConstAccessor()
    {
        if (toArray)
            destroy_at(&itArray);
        else
            destroy_at(&itObject);
    }
    
    const string& Value::ConstAccessor::key()
    {
        if (toArray)
            throw runtime_error("ConstAccessor does not point to an object element, yet key() was called on it");
        
        return itObject->first;
    }
    
    const Value& Value::ConstAccessor::value()
    {
        return toArray ? *itArray : *itObject->second;
    }
}
