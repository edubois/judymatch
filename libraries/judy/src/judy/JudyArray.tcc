
#include "JudyArray.hpp"

namespace judy
{

template <class Key, class T, class Compare, class Allocator>
JudyArray<Key, T, Compare, Allocator>::JudyArray( const std::size_t max_key_length, const std::size_t depth, const Compare& comp, const Allocator& allocator )
: _num_items( 0 )
, _comp( comp )
, _allocator( allocator )
, _max_key_length( max_key_length )
, _buffer( new unsigned char[ _max_key_length ] )
{
    clear_key_buffer();
    assert( sizeof(this) == sizeof(JudySlot) && "Judy array won't work fine as JudySlot hasn't the size of a pointer!" );
    assert( max_key_length > 0 && "You must put a max_key_length greater than 0!" );
    _judy_array = judy_open( max_key_length, depth );
}

template <class Key, class T, class Compare, class Allocator>
JudyArray<Key, T, Compare, Allocator>::JudyArray( const std::size_t max_key_length, const Compare& comp, const Allocator& allocator )
: _num_items( 0 )
, _comp( comp )
, _allocator( allocator )
, _max_key_length( max_key_length )
, _buffer( new unsigned char[ _max_key_length ] )
{
    clear_key_buffer();
    assert( sizeof(this) == sizeof(JudySlot) && "Judy array won't work fine as JudySlot hasn't the size of a pointer!" );
    assert( max_key_length > 0 && "You must put a max_key_length greater than 0!" );
    if ( max_key_length > JUDY_key_size )
    {
        _judy_array = judy_open( max_key_length, 1 );
    }
    else
    {
        _judy_array = judy_open( max_key_length, 0 );
    }
}

template <class Key, class T, class Compare, class Allocator>
JudyArray<Key, T, Compare, Allocator>::JudyArray( const This & other )
{
    operator=(other);
}

template <class Key, class T, class Compare, class Allocator>
JudyArray<Key, T, Compare, Allocator>::~JudyArray()
{
    clear();
    judy_close( _judy_array );
}

template <class Key, class T, class Compare, class Allocator>
typename JudyArray<Key, T, Compare, Allocator>::This& JudyArray<Key, T, Compare, Allocator>::operator=(const JudyArray::This& other)
{
    if ( this == &other )
    {
        return *this;
    }

    // Copy data
    _max_key_length = other._max_key_length;
    _buffer.reset( new unsigned char[ _max_key_length ] );
    _buffer[0] = 0;
    judy_close( _judy_array );
    _judy_array = judy_clone( other._judy_array );
    return *this;
}

template <class Key, class T, class Compare, class Allocator>
bool JudyArray<Key, T, Compare, Allocator>::operator==(const This& other) const
{
    if ( other.size() == size() && size() == 0 )
    {
        return true;
    }

    if ( other.size() != size() && ( size() == 0 || other.size() == 0 ) )
    {
        return false;
    }

    if ( other._max_key_length != _max_key_length )
    {
        return false;
    }

    bool equals = true;
    JudySlot *slot_other = judy_strt( other._judy_array, other._buffer.get(), 0 );
    JudySlot *slot_this = judy_strt( _judy_array, _buffer.get(), 0 );

    while( equals && slot_other != nullptr && slot_this != nullptr )
    {
        // Get keys
        const collisions_set_t *sslot_other = reinterpret_cast<const collisions_set_t *>( *slot_other );
        const collisions_set_t *sslot_this = reinterpret_cast<const collisions_set_t *>( *slot_this );
        other.clear_key_buffer();
        clear_key_buffer();
        judy_key( other._judy_array, other._buffer.get(), other._max_key_length );
        judy_key( _judy_array, _buffer.get(), _max_key_length );
        equals = ( std::memcmp( _buffer.get(), other._buffer.get(), _max_key_length ) == 0 
                   && sslot_other->size() == sslot_this->size() && *sslot_other == *sslot_this );
        // Next step
        slot_other = judy_nxt( other._judy_array );
        slot_this = judy_nxt( _judy_array );
    }

    return equals;
}

template <class Key, class T, class Compare, class Allocator>
bool JudyArray<Key, T, Compare, Allocator>::operator!=(const This& other) const
{
    return !This::operator==(other);
}

template <class Key, class T, class Compare, class Allocator>
void JudyArray<Key, T, Compare, Allocator>::clear()
{
    clear_key_buffer();
    JudySlot *slot = judy_strt( _judy_array, _buffer.get(), 0 );
    bool items_left = (slot != nullptr);
    while( items_left )
    {
        collisions_set_t *sslot = reinterpret_cast<collisions_set_t *>( *slot );
        sslot->clear();
        judy_del( _judy_array );
        clear_key_buffer();
        delete sslot;
        slot = judy_strt( _judy_array, _buffer.get(), 0 );
        items_left = (slot != nullptr);
    }
}

template <class Key, class T, class Compare, class Allocator>
bool JudyArray<Key, T, Compare, Allocator>::empty() const
{
    clear_key_buffer();
    return ( judy_strt( _judy_array, _buffer.get(), 0 ) != nullptr ? false : true );
}

template <class Key, class T, class Compare, class Allocator>
typename JudyArray<Key, T, Compare, Allocator>::iterator JudyArray<Key, T, Compare, Allocator>::find( const key_type& key )
{
    collisions_set_t **slot = reinterpret_cast<collisions_set_t **>( judy_slot( _judy_array, value_pointer( key ), value_length( key ) ) );
    if( slot )
    {
        collisions_set_t * sslot = reinterpret_cast<collisions_set_t *>( *slot );
        return iterator( *this, key, slot, sslot->begin() );
    }
    else
    {
        return end();
    }
}

template <class Key, class T, class Compare, class Allocator>
typename JudyArray<Key, T, Compare, Allocator>::size_type JudyArray<Key, T, Compare, Allocator>::count(const key_type& key) const
{
    const std::size_t klen = value_length( key );
    const JudySlot *slot = judy_slot( _judy_array, value_pointer( key ), klen );
    if ( slot == nullptr )
    { return 0; }

    return ( reinterpret_cast<const collisions_set_t *>( *slot )->size() );
}

template <class Key, class T, class Compare, class Allocator>
typename JudyArray<Key, T, Compare, Allocator>::iterator JudyArray<Key, T, Compare, Allocator>::lower_bound( const key_type& key )
{
    const std::size_t klen = value_length( key );
    collisions_set_t ** slot = reinterpret_cast<collisions_set_t **>( judy_strt( _judy_array, value_pointer( key ), klen ) );
    if ( !slot )
    {
        iterator it = end();
        return --it;
    }

    Key nearest_key;
    resize_value( nearest_key, _max_key_length );
    // Get the key of the first item found
    clear_key_buffer();
    judy_key( _judy_array, _buffer.get(), _max_key_length );
    value_from_pointer( nearest_key, _buffer.get(), false );
    return iterator( *this, nearest_key, slot );
}

template <class Key, class T, class Compare, class Allocator>
typename JudyArray<Key, T, Compare, Allocator>::iterator JudyArray<Key, T, Compare, Allocator>::upper_bound(const key_type& key)
{
    const std::size_t klen = value_length( key );
    JudySlot *slot = judy_strt( _judy_array, value_pointer( key ), klen );
    if ( !slot )
    { return end(); }

    bool items_left = true;
    Key current_key;
    resize_value( current_key, _max_key_length );

    iterator it( _judy_array, key, slot );

    while( items_left )
    {
        clear_key_buffer();
        judy_key( _judy_array, _buffer.get(), _max_key_length );
        value_from_pointer( current_key, _buffer.get(), true );
        // If current item key is greater or equals than key
        if ( _comp( current_key, key, klen ) == false )
        {
            items_left = false;
        }
        else
        {
            items_left = ( it != end() );
            ++it;
        }
    }

    return it;
}

template <class Key, class T, class Compare, class Allocator>
typename JudyArray<Key, T, Compare, Allocator>::const_iterator_pair JudyArray<Key, T, Compare, Allocator>::equal_range(const key_type& key) const
{
    const std::size_t klen = value_length( key );
    // judy_slot is supposed to do the work of equal_range
    JudySlot *slot = reinterpret_cast<JudySlot *>( judy_slot( _judy_array, value_pointer( key ), klen ) );
    if ( !slot )
    { return const_iterator_pair( end(), end() ); }

    collisions_set_t * sslot = reinterpret_cast<collisions_set_t *>( *slot );
    return const_iterator_pair( const_iterator( _judy_array, key, slot ), const_iterator( _judy_array, key, slot, sslot->end() ) );
}

template <class Key, class T, class Compare, class Allocator>
T& JudyArray<Key, T, Compare, Allocator>::operator[]( const key_type& key )
{
    iterator it = find( key );
    if ( it != end() )
    {
        return it->second;
    }
    else
    {
        std::pair<iterator, bool> it_success = insert( value_type( key, T() ) );
        if ( it_success.second )
        {
            return it_success.first->second;
        }
        else
        {
            // This should not happen
            assert( it_success.second == true && "operator[] failed!" );
            throw( std::logic_error( "operator[] failed!" ) );
        }
    }
}

template <class Key, class T, class Compare, class Allocator>
std::pair<typename JudyArray<Key, T, Compare, Allocator>::iterator, bool> JudyArray<Key, T, Compare, Allocator>::insert( const value_type& key_value )
{
    const std::size_t klen = value_length( key_value.first );
    assert( klen <= _max_key_length && "The key length musn't be greater than the maximum length of the judy array's key!" );
    const unsigned char* kptr = value_pointer( key_value.first );
    
    // Insert new item
    collisions_set_t **slot = reinterpret_cast<collisions_set_t **>( judy_cell( _judy_array, kptr, klen ) );
    if ( !slot )
    { throw( std::bad_alloc() ); }

    // This the only doubtful way to store a pointer in a judy array
    if ( *slot == nullptr )
    {
        *slot = new collisions_set_t();
        if ( *slot == nullptr )
        { throw( std::bad_alloc() ); }
    }

    ++_num_items;
    (*slot)->push_back( key_value.second );
    typename collisions_set_t::iterator it = std::prev( (*slot)->end() );
    return std::make_pair( iterator( *this, key_value.first, slot, it ), true );
}

template <class Key, class T, class Compare, class Allocator>
typename JudyArray<Key, T, Compare, Allocator>::iterator JudyArray<Key, T, Compare, Allocator>::erase( const_iterator it )
{
    const Key & key = it->first;
    const std::size_t klen = value_length( key );
    // target the right cell
    JudySlot *slot = judy_slot( _judy_array, value_pointer( key ), klen );
    if ( slot != nullptr )
    {
        const collisions_set_t *sslot = reinterpret_cast<const collisions_set_t *>( *slot );
        delete sslot;
        judy_del( _judy_array );
        --_num_items;
    }
}

template <class Key, class T, class Compare, class Allocator>
typename JudyArray<Key, T, Compare, Allocator>::iterator JudyArray<Key, T, Compare, Allocator>::erase( const_iterator it_begin, const_iterator it_end )
{
    const_iterator it_end2 = it_end;
    std::advance( it_end2 );
    for( auto it = it_begin; it != it_end2; ++it )
    {
        const Key & key = it->first;
        const std::size_t klen = value_length( key );
        JudySlot *slot = judy_slot( _judy_array, value_pointer( key ), klen );
        if ( slot )
        {
            const collisions_set_t *sslot = reinterpret_cast<const collisions_set_t *>( *slot );
            delete sslot;
            judy_del( _judy_array );
            --_num_items;
        }
    }
}

template <class Key, class T, class Compare, class Allocator>
void JudyArray<Key, T, Compare, Allocator>::swap( const This& other )
{
    std::swap( _judy_array, other._judy_array );
}

}
