//
//  iterator.hpp
//  Jsonata
//
//  Copyright © 2015-2016 Dsperados (info@dsperados.com). All rights reserved.
//  Licensed under the BSD 3-clause license.
//

#include <memory>
#include <stdexcept>

#include "value.hpp"

using namespace std;

namespace json
{
    
// --- Iterator --- //
    
	Value::Iterator::Iterator(Array::iterator iterator) :
        toArray(true),
		itArray(iterator)
	{

	}

	Value::Iterator::Iterator(Object::iterator iterator) :
        toArray(false),
		itObject(iterator)
	{
		
	}
    
    Value::Iterator::~Iterator()
    {
        if (toArray)
            std::destroy_at(&itArray);
        else
            std::destroy_at(&itObject);
    }

	Value::Iterator& Value::Iterator::operator++()
	{
        if (toArray)
            ++itArray;
        else
            ++itObject;

		return *this;
	}

	Value::Iterator& Value::Iterator::operator++(int)
	{
        if (toArray)
            ++itArray;
        else
            ++itObject;

		return *this;
	}

	Value::Accessor Value::Iterator::operator*()
	{
        if (toArray)
            return itArray;
        else
            return itObject;
	}

	Value::Accessor Value::Iterator::operator->()
	{
        if (toArray)
            return itArray;
        else
            return itObject;
	}

	bool operator==(const Value::Iterator& lhs, const Value::Iterator& rhs)
	{
        if (lhs.toArray != rhs.toArray)
            return false;
        
        if (lhs.toArray)
            return lhs.itArray == rhs.itArray;
        else
            return lhs.itObject == rhs.itObject;
	}

	bool operator!=(const Value::Iterator& lhs, const Value::Iterator& rhs)
	{
		return !(lhs == rhs);	
	}
    
// --- ConstIterator --- //
    
    Value::ConstIterator::ConstIterator(Array::const_iterator iterator) :
        toArray(true),
        itArray(iterator)
    {
        
    }
    
    Value::ConstIterator::ConstIterator(Object::const_iterator iterator) :
        toArray(false),
        itObject(iterator)
    {
        
    }
    
    Value::ConstIterator::~ConstIterator()
    {
        if (toArray)
            std::destroy_at(&itArray);
        else
            std::destroy_at(&itObject);
    }
    
    Value::ConstIterator& Value::ConstIterator::operator++()
    {
        if (toArray)
            ++itArray;
        else
            ++itObject;
        
        return *this;
    }
    
    Value::ConstIterator& Value::ConstIterator::operator++(int)
    {
        if (toArray)
            ++itArray;
        else
            ++itObject;
        
        return *this;
    }
    
    Value::ConstAccessor Value::ConstIterator::operator*()
    {
        if (toArray)
            return itArray;
        else
            return itObject;
    }
    
    Value::ConstAccessor Value::ConstIterator::operator->()
    {
        if (toArray)
            return itArray;
        else
            return itObject;
    }
    
    bool operator==(const Value::ConstIterator& lhs, const Value::ConstIterator& rhs)
    {
        if (lhs.toArray != rhs.toArray)
            return false;
        
        if (lhs.toArray)
            return lhs.itArray == rhs.itArray;
        else
            return lhs.itObject == rhs.itObject;
    }
    
    bool operator!=(const Value::ConstIterator& lhs, const Value::ConstIterator& rhs)
    {
        return !(lhs == rhs);	
    }
}
