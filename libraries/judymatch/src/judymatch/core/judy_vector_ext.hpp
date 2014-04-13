#ifndef _JM_JUDY_VECTOR_EXT_HPP_
#define	_JM_JUDY_VECTOR_EXT_HPP_

#include <judymatch/config/config.hpp>

#include <judymatch/core/vec_type.hpp>

// This is needed to handle ublas vectors in judy arrays
namespace judy
{

inline const unsigned char *value_pointer( const judymatch::vec_t & value )
{
    return reinterpret_cast<const unsigned char*>( &value[0] );
}

/**
 * Length of a key, in bytes, for an integer key
 * @param
 * @return the length in bytes
 */
inline std::size_t value_length( const judymatch::vec_t & value )
{
    return value.size();
}

/**
 * @brief Get a key object from a pointer
 * @param key the resulting key
 * @param ptr the pointer
 * @param noCopy use ptr as data (don't copy)
 */
inline void value_from_pointer( judymatch::vec_t & value, const unsigned char* ptr, const bool noCopy = false )
{
    const judymatch::real_t * cptr = reinterpret_cast<const std::size_t*>( ptr );
    for( std::size_t i = 0; i < value.size(); ++i )
    {
        value[i] = *(cptr++);
    }
}

}


#endif