#ifndef _JUDYARRAY_JUDYITERATOR_HPP_
#define	_JUDYARRAY_JUDYITERATOR_HPP_

#include "judy.h"
#include "helpers.hpp"
#include <cstddef>
#include <utility>

namespace judy
{

template<class J, class K, class V>
class JudyArrayIterator
{
private:
    typedef JudyArrayIterator<J, K, V> This;

public:
    typedef std::pair<K, V> value_type;
    typedef value_type& reference;

public:
    JudyArrayIterator();
    JudyArrayIterator( const This & other );
    JudyArrayIterator( J & judy_array );
    JudyArrayIterator( J & judy_array, const K & key, const V & value );
    virtual ~JudyArrayIterator();

    bool operator==( const This & other ) const;
    bool operator!=( const This & other ) const;

    inline const value_type & operator*() const
    { return _key_value; }

    inline value_type & operator*()
    { return _key_value; }

    value_type *operator->()
    {
        if ( _valid )
        {
            return &( operator*() );
        }
        else
        {
            return nullptr;
        }
    }

    This &operator++();
    inline This & operator+=( const int n )
    {
        int cpt = 0;
        while( _valid && cpt++ < n )
        {
            operator++();
        }
        return *this;
    }

    This &operator--();
    inline This & operator-=( const int n )
    {
        int cpt = 0;
        while( _valid && cpt++ < n )
        {
            operator--();
        }
        return *this;
    }

private:
    void get_current_key( value_type & key_value );

private:
    J * _judy_array;                                ///< Targeted Judy array
    value_type _key_value;                          ///< Key and value
    bool _valid;                                    ///< Is the iterator pointing to something
};

}

#include "JudyArrayIterator.tcc"

#endif
