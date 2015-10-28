/* Copyright (C) 2012, 2013, 2014, 2015 Eloi DU BOIS - All Rights Reserved
 * The license for this file is available here:
 * https://github.com/edubois/judymatch/blob/master/LICENSE.txt
 */

#ifndef _JM_VEC_TYPE_HPP_
#define	_JM_VEC_TYPE_HPP_

#include <judymatch/config/config.hpp>

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>

namespace judymatch
{

    typedef boost::numeric::ublas::vector<real_t> vec_t;

}

namespace std
{
    inline bool operator<( const judymatch::vec_t& v1, const judymatch::vec_t& v2 )
    {
        for( std::size_t i = 0; i < v1.size(); ++i )
        {
            if ( v1[i] >= v2[i] )
            {
                return false;
            }
        }
        return true;
    }

    inline bool operator>( const judymatch::vec_t& v1, const judymatch::vec_t& v2 )
    {
        for( std::size_t i = 0; i < v1.size(); ++i )
        {
            if ( v1[i] <= v2[i] )
            {
                return false;
            }
        }
        return true;
    }
}

#endif
