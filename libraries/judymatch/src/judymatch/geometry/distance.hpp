#ifndef _JM_DISTANCE_HPP_
#define	_JM_DISTANCE_HPP_

#include <boost/math/special_functions/pow.hpp>

#include <cmath>

namespace judymatch
{
namespace geometry
{

template<class Vec>
const double euclidean_distance( const Vec & a, const Vec & b )
{
    double sum = 0.0;
    const std::size_t imax = std::min( a.size(), b.size() );
    for( std::size_t i = 0; i < imax; ++i )
    {
        sum += boost::math::pow<2>( a[i] - b[i] );
    }
    return std::sqrt( sum );
}

}
}


#endif
