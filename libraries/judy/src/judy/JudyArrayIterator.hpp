/* Copyright (C) 2012, 2013, 2014, 2015 Eloi DU BOIS - All Rights Reserved
 * The license for this file is available here:
 * https://github.com/edubois/judymatch/blob/master/LICENSE.txt
 */

#ifndef _JUDYARRAY_JUDYITERATOR_HPP_
#define	_JUDYARRAY_JUDYITERATOR_HPP_

#include "judy.h"
#include "helpers.hpp"
#include <cstddef>
#include <utility>
#include <memory>

namespace judy
{

template<class J, class K, class V>
class JudyArrayIterator
{
private:
    typedef JudyArrayIterator<J, K, V> This;
    typedef typename J::collisions_set_t collisions_set_t;
public:
    typedef std::pair<K, V> value_type;
    typedef value_type& reference;

public:
    JudyArrayIterator();
    JudyArrayIterator( const This & other );
    JudyArrayIterator( J & judy_array, const bool at_end );
    JudyArrayIterator( J & judy_array, const K & key, collisions_set_t ** slot );
    JudyArrayIterator( J & judy_array, const K & key, collisions_set_t ** slot, const typename collisions_set_t::iterator & it_items_current );
    virtual ~JudyArrayIterator();

    This & operator=( const This & other );
    bool operator==( const This & other ) const;
    bool operator!=( const This & other ) const;

    inline const value_type operator*() const
    { return value_type( *_key, *_it_items_current ); }

    inline value_type operator*()
    { return value_type( *_key, *_it_items_current ); }

    value_type *operator->()
    {
        if ( _key )
        {
            static value_type v;
            v = operator*();
            return &v;
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
        while( _key && cpt++ < n )
        {
            operator++();
        }
        return *this;
    }

    This &operator--();
    inline This & operator-=( const int n )
    {
        int cpt = 0;
        while( _key && cpt++ < n )
        {
            operator--();
        }
        return *this;
    }

private:
    void backup_state();
    void restore_state();
    void get_current_key();
    bool get_items_at( collisions_set_t ** slot );
private:
    J * _judy_array;                                ///< Targeted Judy array
    JudyStack _judy_stack;                          ///< Current judy array stack (cursor)
    std::unique_ptr<K> _key;                        ///< Key
    bool _at_end;                                   ///< Needed to identify end iterator
    typename collisions_set_t::iterator _it_items_begin;     ///< Begin iterator associated to a fetched collision set
    typename collisions_set_t::iterator _it_items_current;   ///< Current iterator associated to a fetched collision set
    typename collisions_set_t::iterator _it_items_end;       ///< End iterator associated to a fetched collision set
};

}

#include "JudyArrayIterator.tcc"

#endif
