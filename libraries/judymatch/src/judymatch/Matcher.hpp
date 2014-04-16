#ifndef _JM_MATCHER_HPP_
#define	_JM_MATCHER_HPP_

#include <judymatch/geohash/dct/dct.hpp>
#include <judymatch/geohash/spiroid/spiroid.hpp>
#include <judy/JudyArray.hpp>

namespace judymatch
{

/**
 * @brief Very fast pattern matching algorithm, using geospatial hashing and 
 * judy arrays
 */
template<class T, class H>
class Matcher
{
/*
 * typedefs
 */
private:
    typedef Matcher<T, H> This;

public:
    typedef typename H::value_type hash_value;
    typedef judy::JudyArray<hash_value, T> database_t;
    typedef typename database_t::iterator iterator;
    typedef typename database_t::const_iterator const_iterator;

/**
 * methods
 */
public:
    Matcher( const H & geohasher );
    virtual ~Matcher();

    inline iterator end()
    { return _database.end(); }

    inline const_iterator end() const
    { return _database.end(); }

    inline iterator begin()
    { return _database.begin(); }

    inline const_iterator begin() const
    { return _database.begin(); }

    inline void insert( const T & vec )
    { _database.insert( std::make_pair( _geohasher.hash_for_construction( vec ), vec ) ); }

    inline iterator closest( const T & vec )
    { return _database.lower_bound( _geohasher.hash_for_find_back( vec ) ); }

    inline iterator find( const T & vec )
    { return _database.find( _geohasher.hash_for_find_back( vec ) ); }

    inline const std::size_t size() const
    { return _database.size(); }

protected:
    database_t _database;  //< Judy array as a database
    H _geohasher;

};

}

#include "Matcher.tcc"

#endif
