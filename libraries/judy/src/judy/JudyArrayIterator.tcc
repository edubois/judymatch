#include "JudyArrayIterator.hpp"
#include <iostream>

namespace judy
{

template<class J, class K, class V>
JudyArrayIterator<J, K, V>::JudyArrayIterator()
: _judy_array( nullptr )
, _at_end( true )
{
}

template<class J, class K, class V>
JudyArrayIterator<J, K, V>::JudyArrayIterator( const This & other )
{
    operator=( other );
}

template<class J, class K, class V>
JudyArrayIterator<J, K, V>::JudyArrayIterator( J & judy_array, const bool at_end )
: _judy_array( &judy_array )
, _at_end( at_end )
{
    if ( at_end == false )
    {
        // Get the first element of the judy array, if existing
        if ( get_items_at( reinterpret_cast<collisions_set_t **>( judy_strt( _judy_array->_judy_array, _judy_array->_buffer.get(), 0 ) ) ) )
        {
            get_current_key();
        }
    }
}

template<class J, class K, class V>
JudyArrayIterator<J, K, V>::JudyArrayIterator( J & judy_array, const K & key, collisions_set_t ** slot, const typename collisions_set_t::iterator & it_items_current )
: _judy_array( &judy_array )
, _at_end( false )
{
    if ( get_items_at( slot ) )
    {
        _key.reset( new K( key ) );
        _it_items_current = it_items_current;
    }
}

template<class J, class K, class V>
JudyArrayIterator<J, K, V>::JudyArrayIterator( J & judy_array, const K & key, collisions_set_t ** slot )
: _judy_array( &judy_array )
, _at_end( false )
{
    if ( get_items_at( slot ) )
    {
        _key.reset( new K( key ) );
    }
}

template<class J, class K, class V>
JudyArrayIterator<J, K, V>::~JudyArrayIterator()
{
}

template<class J, class K, class V>
typename JudyArrayIterator<J, K, V>::This & JudyArrayIterator<J, K, V>::operator=( const This & other )
{
    _judy_array = other._judy_array;
    _it_items_begin = other._it_items_begin;
    _it_items_current = other._it_items_current;
    _it_items_end = other._it_items_end;
    _at_end = other._at_end;

    if ( other._key )
    {
        _key.reset( new K( *other._key ) );
    }
    
    return *this;
}

template<class J, class K, class V>
bool JudyArrayIterator<J, K, V>::operator==( const This & other ) const
{
    if ( _at_end == other._at_end && _at_end == true )
    { return true; }

    if ( _key == nullptr && other._key == nullptr )
    { return true; }

    if ( _key == nullptr || other._key == nullptr )
    { return false; }

    if ( ( _it_items_current == _it_items_end ) || ( _it_items_current == other._it_items_end ) )
    {
        return _key == other._key;
    }

    return ( equals( *_key, *other._key ) && ( equals( *_it_items_current, *other._it_items_current ) ) );
}

template<class J, class K, class V>
bool JudyArrayIterator<J, K, V>::operator!=( const This & other ) const
{
    return !operator==( other );
}

template<class J, class K, class V>
typename JudyArrayIterator<J, K, V>::This &JudyArrayIterator<J, K, V>::operator++()
{
    if ( _judy_array == nullptr )
    { 
        _key.reset();
        return *this;
    }

    if ( _key && _it_items_current != _it_items_end )
    {
        ++_it_items_current;
        if ( _it_items_current != _it_items_end )
        {
            return *this;
        }
        else
        {
            _key.reset();
        }
    }

    if ( get_items_at( reinterpret_cast<collisions_set_t **>( judy_nxt( _judy_array->_judy_array ) ) ) )
    {
        get_current_key();
    }
    else
    {
        _key.reset();
    }

    return *this;
}

template<class J, class K, class V>
typename JudyArrayIterator<J, K, V>::This &JudyArrayIterator<J, K, V>::operator--()
{
    if ( _judy_array == nullptr )
    {
        _key.reset();
        return *this;
    }
    
    if ( _at_end )
    {
        // Get the first element of the judy array, if existing
        if ( get_items_at( reinterpret_cast<collisions_set_t **>( judy_end( _judy_array->_judy_array ) ) ) )
        {
            get_current_key();
            _it_items_current = std::prev( _it_items_end );
            _at_end = false;
        }
        return *this;
    }

    if ( _key && _it_items_current != _it_items_begin )
    {
        --_it_items_current;
        return *this;
    }

    if ( get_items_at( reinterpret_cast<collisions_set_t **>( judy_prv( _judy_array->_judy_array ) ) ) )
    {
        get_current_key();
        _it_items_current = std::prev( _it_items_end );
    }
    else
    {
        _key.reset();
    }

    return *this;
}

template<class J, class K, class V>
bool JudyArrayIterator<J, K, V>::get_items_at( collisions_set_t **slot )
{
    if ( !slot )
    { return false; }

    collisions_set_t *sslot = *slot;
    if ( sslot )
    {
        _it_items_begin = sslot->begin();
        _it_items_current = sslot->begin();
        _it_items_end = sslot->end();
        return true;
    }
    return false;
}

template<class J, class K, class V>
void JudyArrayIterator<J, K, V>::get_current_key()
{
    _judy_array->clear_key_buffer();
    judy_key( _judy_array->_judy_array, _judy_array->_buffer.get(), _judy_array->_max_key_length );
    _key.reset( new K() );
    value_from_pointer( *_key, _judy_array->_buffer.get(), false );
}

}
