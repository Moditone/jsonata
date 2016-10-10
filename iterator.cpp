//
//  iterator.hpp
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
    
// --- Iterator --- //
    
	Value::Iterator::Iterator(Array::iterator iterator) :
		iterator(iterator)
	{

	}

	Value::Iterator::Iterator(Object::iterator iterator) :
		iterator(iterator)
	{
		
	}

	Value::Iterator& Value::Iterator::operator++()
	{
		switch (iterator.which())
		{
			case 0: ++boost::get<Array::iterator>(iterator); break;
			case 1: ++boost::get<Object::iterator>(iterator); break;
		}

		return *this;
	}

	Value::Iterator& Value::Iterator::operator++(int)
	{
		switch (iterator.which())
		{
			case 0: boost::get<Array::iterator>(iterator)++; break;
			case 1: boost::get<Object::iterator>(iterator)++; break;
		}

		return *this;
	}

	Value::Accessor Value::Iterator::operator*()
	{
		return {*this};
	}

	Value::Accessor Value::Iterator::operator->()
	{
		return {*this};
	}

	bool operator==(const Value::Iterator& lhs, const Value::Iterator& rhs)
	{
		return lhs.asVariant() == rhs.asVariant();
	}

	bool operator!=(const Value::Iterator& lhs, const Value::Iterator& rhs)
	{
		return !(lhs == rhs);	
	}
    
// --- ConstIterator --- //
    
    Value::ConstIterator::ConstIterator(Array::const_iterator iterator) :
        iterator(iterator)
    {
        
    }
    
    Value::ConstIterator::ConstIterator(Object::const_iterator iterator) :
        iterator(iterator)
    {
        
    }
    
    Value::ConstIterator& Value::ConstIterator::operator++()
    {
        switch (iterator.which())
        {
            case 0: ++boost::get<Array::const_iterator>(iterator); break;
            case 1: ++boost::get<Object::const_iterator>(iterator); break;
        }
        
        return *this;
    }
    
    Value::ConstIterator& Value::ConstIterator::operator++(int)
    {
        switch (iterator.which())
        {
            case 0: boost::get<Array::const_iterator>(iterator)++; break;
            case 1: boost::get<Object::const_iterator>(iterator)++; break;
        }
        
        return *this;
    }
    
    Value::ConstAccessor Value::ConstIterator::operator*()
    {
        return {*this};
    }
    
    Value::ConstAccessor Value::ConstIterator::operator->()
    {
        return {*this};
    }
    
    bool operator==(const Value::ConstIterator& lhs, const Value::ConstIterator& rhs)
    {
        return lhs.asVariant() == rhs.asVariant();
    }
    
    bool operator!=(const Value::ConstIterator& lhs, const Value::ConstIterator& rhs)
    {
        return !(lhs == rhs);	
    }
}
