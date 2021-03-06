/* Copyright (C) 2012, 2013, 2014, 2015 Eloi DU BOIS - All Rights Reserved
 * The license for this file is available here:
 * https://github.com/edubois/judymatch/blob/master/LICENSE.txt
 */

/* JudyMatcher fast pattern matching library
 * Copyright (C) 2014 Eloi Du Bois
 * eloi.du.bois@gmail.com
 *
 * Rework of an idea I had in 2009.
 * 
 * PLEASE PUT MY NAME IN YOUR SPECIAL THANKS IF YOU USE THIS LIB
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for  more details.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 */

#ifndef _JM_MATCHER_HPP_
#define	_JM_MATCHER_HPP_

#include <judy/JudyArray.hpp>

#include <judymatch/config/global.hpp>
#include <judymatch/geometry/distance.hpp>

#include <map>

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
    using This = Matcher<T, H>;

public:
    using hash_value = typename H::value_type;
#ifdef USE_JUDY_ARRAY_T
    using database_t = judy::JudyArray<hash_value, T>;
#else
    using database_t = std::multimap<hash_value, T, std::less<hash_value> >;
#endif
    using iterator = typename database_t::iterator;
    using reverse_iterator = typename database_t::reverse_iterator;
    using const_iterator = typename database_t::const_iterator;

/**
 * methods
 */
public:
    Matcher( const H & geohasher );
    virtual ~Matcher();

    inline iterator rend()
    { return _database.rend(); }

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

    inline iterator closest_precise( const T & vec, const std::size_t window_size )
    {
        iterator it = closest( vec );
        iterator rit = it;
        if ( it != end() )
        {
            double d_min = geometry::euclidean_distance(it->second, vec);
            iterator it_min = it;
            for( std::size_t i = 0; i < window_size && it != end(); ++i )
            {
                const double d = geometry::euclidean_distance(it->second, vec);
                if ( d < d_min )
                {
                    d_min = d;
                    it_min = it;
                }
                ++it;
            }
            iterator rit_min = rit;
            for( std::size_t i = 0; i < window_size && rit != begin(); ++i )
            {
                const double d = geometry::euclidean_distance(rit->second, vec);
                if ( d < d_min )
                {
                    d_min = d;
                    rit_min = rit;
                }
                --rit;
            }
            const double d1 = geometry::euclidean_distance(it_min->second, vec);
            const double d2 = geometry::euclidean_distance(rit_min->second, vec);
            if ( d1 < d2 )
            {
                return it_min;
            }
            else
            {
                return rit_min;
            }
        }
        return it;
    }

    inline iterator closest( const T & vec )
    {
        const typename H::value_type h = _geohasher.hash_for_find_back( vec );
        iterator upper = _database.lower_bound( h );
        if ( upper == _database.begin() || ( upper != _database.end() && geometry::euclidean_distance(upper->first, h) == 0.0 ) )
        {
            return upper;
        }
        iterator lower = upper;
        const double dist_upper = ( upper != _database.end() ) ? geometry::euclidean_distance(upper->first, h) : 0.0;
        --lower;
        if ( upper == _database.end() ||
                ( geometry::euclidean_distance(h, lower->first) < dist_upper ) )
        {
            return lower;
        }
        return upper;
    }

    inline iterator find( const T & vec )
    { return _database.find( _geohasher.hash_for_find_back( vec ) ); }

    inline const std::size_t size() const
    { return _database.size(); }

    /**
     * Needed because you sometimes need to do specific operations such as 'reserve'
     * @return database structure
     */
    inline database_t & database()
    { return _database; }

protected:
    database_t _database;  //< Judy array as a database
    H _geohasher;

};

}

#include "Matcher.tcc"

#endif
