
#include "JudyArray.hpp"

namespace judy
{

template <class Key, class T, class Compare, class Allocator>
JudyArray<Key, T, Compare, Allocator>::JudyArray( const std::size_t max_key_length, const Compare& comp, const Allocator& allocator )
: _num_items( 0 )
, _comp( comp )
, _allocator( allocator )
, _max_key_length( max_key_length )
, _buffer( new unsigned char[ _max_key_length ] )
{
    clear_key_buffer();
    assert( max_key_length > 0 && "You must put a max_key_length greater than 0!" );
    _judy_array = judy_open( max_key_length, 1 );
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
    const JudySlot *slotOther = judy_strt( other._judy_array, other._buffer.get(), 0 );
    const JudySlot *slotThis = judy_strt( _judy_array, _buffer.get(), 0 );

    while( equals && slotOther != nullptr && slotThis != nullptr )
    {
        // Get keys
        other.clear_key_buffer();
        clear_key_buffer();
        judy_key( other._judy_array, other._buffer.get(), other._max_key_length );
        judy_key( _judy_array, _buffer.get(), _max_key_length );
        equals = ( std::memcmp( _buffer.get(), other._buffer.get(), _max_key_length ) == 0 
                    && *slotOther == *slotThis );
        // Next step
        slotOther = judy_nxt( other._judy_array );
        slotThis = judy_nxt( _judy_array );
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
    const JudySlot *slot = judy_strt( _judy_array, _buffer.get(), 0 );
    bool items_left = (slot != nullptr);
    while( items_left )
    {
        judy_del( _judy_array );
        clear_key_buffer();
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
    unsigned char *slot = reinterpret_cast<unsigned char *>( judy_slot( _judy_array, value_pointer( key ), value_length( key ) ) );
    if( slot )
    {
        T val;
        value_from_pointer( val, slot, false );
        return iterator( *this, key, val );
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
    const unsigned char *slot = reinterpret_cast<const unsigned char *>( judy_slot( _judy_array, value_pointer( key ), klen ) );
    if ( slot == nullptr )
    { return 0; }

    bool items_left = true;
    Key current_key;

    std::size_t cnt = 0;
    while( items_left )
    {
        clear_key_buffer();
        judy_key( _judy_array, _buffer.get(), _max_key_length );
        value_from_pointer( current_key, _buffer.get(), true );
        if ( current_key != key )
        {
            items_left = false;
        }
        else
        {
            ++cnt;
            slot = reinterpret_cast<const unsigned char *>( judy_nxt( _judy_array ) );
            items_left = (slot != nullptr);
        }
    }
    
    return cnt;
}

template <class Key, class T, class Compare, class Allocator>
typename JudyArray<Key, T, Compare, Allocator>::iterator JudyArray<Key, T, Compare, Allocator>::lower_bound( const key_type& key )
{
    const std::size_t klen = value_length( key );
    const T *slot = reinterpret_cast<const unsigned char *>( judy_strt( _judy_array, value_pointer( key ), klen ) );
    Key nearest_key;
    T nearest_value;
    // Get the key of the first item found
    clear_key_buffer();
    judy_key( _judy_array, _buffer.get(), _max_key_length );
    value_from_pointer( nearest_key, _buffer.get(), false );
    value_from_pointer( nearest_value, slot, false );
    return iterator( *this, nearest_key, nearest_value );
}

template <class Key, class T, class Compare, class Allocator>
typename JudyArray<Key, T, Compare, Allocator>::iterator JudyArray<Key, T, Compare, Allocator>::upper_bound(const key_type& key)
{
    const std::size_t klen = value_length( key );
    const unsigned char *slot = reinterpret_cast<const unsigned char *>( judy_strt( _judy_array, value_pointer( key ), klen ) );
    if ( !slot )
    { return end(); }

    bool items_left = true;
    Key current_key;
    T current_value;

    value_from_pointer( current_value, slot, true );
    iterator it( _judy_array, key, current_value );

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
    const unsigned char *slot = reinterpret_cast<const unsigned char *>( judy_slot( _judy_array, value_pointer( key ), klen ) );
    if ( !slot )
    {
        return const_iterator_pair( end(), end() );
    }

    T current_value;
    value_from_pointer( current_value, slot, false );

    const_iterator it_begin( _judy_array, key, current_value );
    const_iterator it = it_begin;
    const_iterator it_last = it_begin;
    while( it != end() )
    {
        it_last = it;
        ++it;
    }

    return const_iterator_pair( it_begin, it_last );
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
            ++_num_items;
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

    // Disallow duplicate keys
    {
        iterator itExisting = find( key_value.first );
        if ( itExisting != end() )
        {
            std::cerr << "Item already exists!" << std::endl;
            return std::make_pair( itExisting, false );
        }
    }

    // Insert new item
    unsigned char *slot = reinterpret_cast<unsigned char *>( judy_cell( _judy_array, kptr, klen ) );
    if( slot )
    {
        memcpy( slot, value_pointer( key_value.second ), value_length( key_value.second ) );
        ++_num_items;
        T current_value;
        value_from_pointer( current_value, slot, false );
        return std::make_pair( iterator( *this, key_value.first, current_value ), true );
    }
    else
    {
        return std::make_pair( end(), false );
    }
}

template <class Key, class T, class Compare, class Allocator>
typename JudyArray<Key, T, Compare, Allocator>::iterator JudyArray<Key, T, Compare, Allocator>::erase( const_iterator it )
{
    const Key & key = it->first;
    const std::size_t klen = value_length( key );
    // target the right cell
    if ( judy_slot( _judy_array, value_pointer( key ), klen ) )
    {
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
        const JudySlot *slot = reinterpret_cast<const JudySlot *>( judy_slot( _judy_array, value_pointer( key ), klen ) );
        if ( slot )
        {
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
