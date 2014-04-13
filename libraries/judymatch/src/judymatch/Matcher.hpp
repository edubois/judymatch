#ifndef _JM_MATCHER_HPP_
#define	_JM_MATCHER_HPP_

#include <judymatch/geohash/dct/dct.hpp>
#include <judy/JudyArray.hpp>

namespace judymatch
{

/**
 * @brief Very fast pattern matching algorithm, using geospatial hashing and 
 * judy arrays
 */
template<class DataT>
class Matcher
{
/*
 * typedefs
 */
private:
    typedef Matcher<DataT> This;

public:
    Matcher();
    virtual ~Matcher();

protected:
    judy::JudyArray<DataT, DataT> _database;  //< Judy array as a database

};

}

#include "Matcher.tcc"

#endif
