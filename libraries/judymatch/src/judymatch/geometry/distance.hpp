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
    BOOST_ASSERT( a.size() == b.size() );
    double sum = 0.0;
    for( std::size_t i = 0; i < a.size(); ++i )
    {
        sum += boost::math::pow<2>( a[i] - b[i] );
    }
    return std::sqrt( sum );
}

}
}


#endif
