#ifndef _JUDY_HELPERS_HPP
#define	_JUDY_HELPERS_HPP

#include <string>
#include <vector>
#include <iostream>
#include <cassert>

namespace judy
{

template<typename T>
inline void resize_value( std::vector<T> & v1, const std::size_t sz )
{
    v1.resize( sz );
}

template<typename T>
inline bool equals( const std::vector<T> & v1, const std::vector<T> & v2 )
{
    return v1 == v2;
}

template<typename T>
inline const unsigned char *value_pointer( const std::vector<T> & array )
{
    if ( array.size() )
    {
        return reinterpret_cast<const unsigned char*>( &array[0] );
    }
    else
    {
        return nullptr;
    }
}

template<typename T>
void clear_buffer( unsigned char * ptr, const std::size_t len )
{
    memset( ptr, 0, len );
}

template<>
void clear_buffer<std::string>( unsigned char * ptr, const std::size_t len );

/**
 * Length of a key, in bytes, for a vector<T> key
 * @param array input vector
 * @return the length in bytes
 */
template<typename T>
inline std::size_t value_length( const std::vector<T> & array )
{
    return array.size() * sizeof( T );
}
//////////////////////// INTEGERS ///////////////////

inline void resize_value( std::size_t, const std::size_t )
{
}

inline bool equals( const std::size_t v1, const std::size_t v2 )
{
    return v1 == v2;
}

inline const unsigned char *value_pointer( const std::size_t & value )
{
    return reinterpret_cast<const unsigned char*>( &value );
}

/**
 * Length of a key, in bytes, for an integer key
 * @param
 * @return the length in bytes
 */
inline std::size_t value_length( const std::size_t )
{
    return sizeof( std::size_t );
}

/**
 * @brief Get a key object from a pointer
 * @param key the resulting key
 * @param ptr the pointer
 * @param noCopy use ptr as data (don't copy)
 */
inline void value_from_pointer( std::size_t & value, const unsigned char* ptr, const bool noCopy = false )
{
    const std::size_t* cptr = reinterpret_cast<const std::size_t*>( ptr );
    value = *cptr;
}

//////////////////////// DOUBLES ///////////////////

inline void resize_value( double, const std::size_t )
{
}

inline bool equals( const double v1, const double v2 )
{
    return v1 == v2;
}

inline const unsigned char *value_pointer( const double & value )
{
    return reinterpret_cast<const unsigned char*>( &value );
}

/**
 * Length of a key, in bytes, for an integer key
 * @param
 * @return the length in bytes
 */
inline std::size_t value_length( const double )
{
    return sizeof( double );
}

/**
 * @brief Get a key object from a pointer
 * @param key the resulting key
 * @param ptr the pointer
 * @param noCopy use ptr as data (don't copy)
 */
inline void value_from_pointer( double & value, const unsigned char* ptr, const bool noCopy = false )
{
    const double* cptr = reinterpret_cast<const double*>( ptr );
    value = *cptr;
}

/////////////////////// STRINGS /////////////////////
inline void resize_value( std::string & v, const std::size_t sz )
{
}

inline bool equals( const std::string & s1, const std::string & s2 )
{
    return s1 == s2;
}

inline const unsigned char *value_pointer( const std::string & str )
{
    return reinterpret_cast<const unsigned char*>( str.c_str() );
}

/**
 * Length of a key, in bytes, for a string key
 * @param str input string
 * @return the length in bytes
 */
inline std::size_t value_length( const std::string & str )
{
    return str.size() + 1;
}

/**
 * @brief Get a key object from a pointer
 * @param key the resulting key
 * @param ptr the pointer
 * @param noCopy use ptr as data (don't copy)
 */
inline void value_from_pointer( std::string & value, const unsigned char* ptr, const bool noCopy = false )
{
    const char* cptr = reinterpret_cast<const char*>( ptr );
    value = cptr;
    assert( value.c_str() != cptr );
}

}

#endif
