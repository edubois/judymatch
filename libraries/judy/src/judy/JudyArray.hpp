/**
 * @brief This is a wrapper for judy arrays, done by Eloi du Bois (c) 2014.
 * Please read license of judymatch library for informations about the license.
 * 
 * The main idea is to follow the recommandations of the C++ 11 standard to make
 * a Judy Array data structure that you can manipulate just as a multimap, but with
 * all pros from a judy array.
 * 
 * @warning THIS IS EXPERIMENTAL, SOME FEATURES ARE NOT WORKING (in certain contexts).
 * The reason is _judy_array is using a self owned state that I can't restore 
 * after doing operations (in particular when using iterators).
 * My advise would be to use one iterator per time
 */
#ifndef _JUDY_JUDYARRAY_HPP_
#define	_JUDY_JUDYARRAY_HPP_

#include "JudyArrayIterator.hpp"
#include "helpers.hpp"
#include "judy.h"

#include <cassert>
#include <memory>
#include <utility>
#include <vector>
#include <string>

namespace judy
{

/**
 * @brief judy arrays C++ wrapper
 */
template <class Key, class T, class Compare = std::less<Key>, class Allocator = std::allocator< std::pair<const Key, T> > >
class JudyArray
{
/*
 * typedefs
 */
private:
    typedef JudyArray<Key, T, Compare, Allocator> This;
public:
    typedef std::vector<T> collisions_set_t;
    typedef Key key_type;
    typedef T mapped_type;
    typedef typename JudyArrayIterator<This, Key, T>::value_type value_type;
    typedef Compare key_compare;
    typedef Allocator allocator_type;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef JudyArrayIterator<This, Key, T> iterator;
    typedef JudyArrayIterator<const This, Key, T> const_iterator;
    typedef std::size_t size_type;
    typedef typename Allocator::difference_type difference_type;
    typedef std::pair<iterator,iterator> iterator_pair;
    typedef std::pair<const_iterator,const_iterator> const_iterator_pair;

    typedef typename std::allocator_traits<Allocator>::pointer pointer;
    typedef typename std::allocator_traits<Allocator>::const_pointer const_pointer;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    friend class JudyArrayIterator<This, Key, T>;
public:
    class value_compare
    {
        friend class map;
        protected:
            Compare comp;
            value_compare(Compare c) : comp(c) {}
        public:
            typedef bool result_type;
            typedef value_type first_argument_type;
            typedef value_type second_argument_type;

            inline bool operator()(const value_type& x, const value_type& y) const
            {
                return comp( x.first, y.first );
            }
    };

public:
    // 23.4.4.2, construct/copy/destroy:
    /**
     * @brief constructor
     * @param max_key_length max key length in bytes
     * @param comp comparator
     * @param Allocator allocator
     */
    explicit JudyArray( const size_t max_key_length, const Compare& comp = Compare(), const Allocator& = Allocator() );
    JudyArray( const This & other );
    virtual ~JudyArray();

    This& operator=(const This&);
    bool operator==(const This&) const;
    bool operator!=(const This&) const;

    inline iterator begin()
    { return iterator( *this, false ); }

    inline const_iterator begin() const
    { return const_iterator( *this, false ); }

    inline const_iterator cbegin() const
    { return const_iterator( *this, false ); }

    inline iterator end()
    { return iterator( *this, true ); }

    inline const_iterator end() const
    { return const_iterator( *this, true ); }

    inline const_iterator cend() const
    { return const_iterator( *this, true ); }

    inline size_type size() const
    { return _num_items; }

    inline size_type max_size() const
    { return static_cast<size_type>( -1 ); }

    bool empty() const;

    inline Allocator get_allocator() const
    { return _allocator; }

    // observers:
    inline key_compare key_comp() const
    { return _comp; }
    inline value_compare value_comp() const
    { return _comp; }

    // 23.4.4.5, map operations:
    iterator find(const key_type& key);
    const_iterator find(const key_type& key) const
    { return (const_cast<This*>(this))->find( key ); }
    size_type count(const key_type& key) const;

    iterator lower_bound(const key_type& key);
    inline const_iterator lower_bound(const key_type& key) const
    { return ( const_cast<This*>( this ) )->lower_bound( key ); }
    iterator upper_bound(const key_type& key);
    inline const_iterator upper_bound(const key_type& key) const
    { return (const_cast<This*>(this))->upper_bound( key ); }

    const_iterator_pair equal_range(const key_type& x) const;

    // 23.4.4.3, element access:
    T& operator[](const key_type& x);
    T& operator[](key_type&& x);
    T& at(const key_type& x);
    const T& at(const key_type& x) const;

    //23.4.4.4, modifiers:
    std::pair<iterator, bool> insert(const value_type& x);
    iterator erase(const_iterator);
    iterator erase(const_iterator, const_iterator);
    void clear();
    void swap(const This&);

private:
    inline void clear_key_buffer() const
    { clear_buffer<Key>( _buffer.get(), _max_key_length ); }

private:
    size_type _num_items;                           ///< Number of items in the container
    const Compare _comp;                            ///< Comparison object
    const Allocator _allocator;                     ///< Allocator object
    Judy * _judy_array;                             ///< Karl Malbrain data structure, @see judy.h
    std::size_t _max_key_length;                    ///< Maximum key length
    std::unique_ptr<unsigned char[]> _buffer;       ///< Used for data access
};
    
}

#include "JudyArray.tcc"

#endif
