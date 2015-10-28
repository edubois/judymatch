/* Copyright (C) 2012, 2013, 2014, 2015 Eloi DU BOIS - All Rights Reserved
 * The license for this file is available here:
 * https://github.com/edubois/judymatch/blob/master/LICENSE.txt
 */

#ifndef _JM_JUDY_VECTOR_EXT_HPP_
#define	_JM_JUDY_VECTOR_EXT_HPP_

#include <judymatch/config/config.hpp>

#include <judymatch/core/vec_type.hpp>

// This is needed to handle ublas vectors in judy arrays
namespace judy
{

inline const unsigned char *value_pointer( const judymatch::vec_t & value )
{
    return reinterpret_cast<const unsigned char*>( &(value[0]) );
}

/**
 * Length of a key, in bytes, for an integer key
 * @param
 * @return the length in bytes
 */
inline std::size_t value_length( const judymatch::vec_t & value )
{
    return value.size() * sizeof( judymatch::vec_t::value_type );
}

/**
 * @brief Get a key object from a pointer
 * @param key the resulting key
 * @param ptr the pointer
 * @param noCopy use ptr as data (don't copy)
 */
inline void value_from_pointer( judymatch::vec_t & value, const unsigned char* ptr, const bool noCopy = false )
{
    const judymatch::real_t * cptr = reinterpret_cast<const judymatch::real_t *>( ptr );
    for( std::size_t i = 0; i < value.size(); ++i )
    {
        value[i] = *(cptr++);
    }
}

inline bool equals( const judymatch::vec_t & v1, const judymatch::vec_t & v2 )
{
    if ( value_length( v1 ) != value_length( v2 ) )
    {
        return false;
    }
    else
    {
        return memcmp( value_pointer( v1 ), value_pointer( v2 ), value_length( v1 ) ) == 0;
    }
}

inline void resize_value( judymatch::vec_t & v, const std::size_t sz )
{
    v.resize( sz / sizeof( judymatch::vec_t::value_type ) );
}

}


#endif
