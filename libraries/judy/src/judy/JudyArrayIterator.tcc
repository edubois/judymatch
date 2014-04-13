
#include "JudyArrayIterator.hpp"
#include <iostream>

namespace judy
{

template<class J, class K, class V>
JudyArrayIterator<J, K, V>::JudyArrayIterator()
: _judy_array( nullptr )
, _valid( false )
{
}

template<class J, class K, class V>
JudyArrayIterator<J, K, V>::JudyArrayIterator( const This & other )
: _judy_array( other._judy_array )
, _valid( other._valid )
, _key_value( other._key_value )
{
}

template<class J, class K, class V>
JudyArrayIterator<J, K, V>::JudyArrayIterator( J & judy_array )
: _judy_array( &judy_array )
{
    // Get the first element
    const unsigned char *slot = reinterpret_cast<const unsigned char *>( judy_strt( _judy_array->_judy_array, _judy_array->_buffer.get(), 0 ) );
    if ( slot )
    {
        value_from_pointer( _key_value.second, slot, false );
        // Get its key
        get_current_key( _key_value );
        _valid = true;
    }
    else
    {
        _valid = false;
    }
}

template<class J, class K, class V>
JudyArrayIterator<J, K, V>::JudyArrayIterator( J & judy_array, const K & key, const V & value )
: _judy_array( &judy_array )
, _key_value( key, value )
, _valid( true )
{
}

template<class J, class K, class V>
JudyArrayIterator<J, K, V>::~JudyArrayIterator()
{
}

template<class J, class K, class V>
bool JudyArrayIterator<J, K, V>::operator==( const This & other ) const
{
    if ( _valid == false && other._valid == false )
    { return true; }
    
    if ( _valid != other._valid )
    { return false; }

    return _key_value == other._key_value;
}

template<class J, class K, class V>
bool JudyArrayIterator<J, K, V>::operator!=( const This & other ) const
{
    if ( _valid == false && other._valid == false )
    { return false; }

    if ( _valid != other._valid )
    { return true; }

    return _key_value != other._key_value;
}

template<class J, class K, class V>
typename JudyArrayIterator<J, K, V>::This &JudyArrayIterator<J, K, V>::operator++()
{
    if ( _judy_array == nullptr )
    { 
        _valid = false;
        return *this;
    }

    V* slot = reinterpret_cast<V *>( judy_nxt( _judy_array->_judy_array ) );
    if ( slot )
    {
        get_current_key( _key_value );
        _key_value.second = *slot;
        _valid = true;
    }
    else
    {
        _valid = false;
    }

    return *this;
}

template<class J, class K, class V>
typename JudyArrayIterator<J, K, V>::This &JudyArrayIterator<J, K, V>::operator--()
{
    if ( _judy_array == nullptr )
    { 
        _valid = false;
        return *this;
    }

    V* slot = reinterpret_cast<V *>( judy_prv( _judy_array->_judy_array ) );
    if ( slot )
    {
        get_current_key( _key_value );
        _key_value.second = *slot;
        _valid = true;
    }
    else
    {
        _valid = false;
    }

    return *this;
}

template<class J, class K, class V>
void JudyArrayIterator<J, K, V>::get_current_key( value_type & key_value )
{
    _judy_array->clear_key_buffer();
    judy_key( _judy_array->_judy_array, _judy_array->_buffer.get(), _judy_array->_max_key_length );
    value_from_pointer( key_value.first, _judy_array->_buffer.get(), false );
}

}
