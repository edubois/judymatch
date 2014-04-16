#include "spiroid.hpp"

#include <judymatch/config/global.hpp>

namespace judymatch
{
namespace geohash
{
namespace spiroid
{

double spiroid_geohasher::loc_spiroid_fft( const judymatch::vec_t & vec_in, const double min_dist, const double translation ) const
{
    BOOST_ASSERT( vec_in.size() );
    static std::vector<cpx_t> vec_in_cpx;
    vec_in_cpx.resize( vec_in.size() );

    {
        const double x = vec_in[0] - translation;
        const double c = std::abs(x) * cos(x) / ( 2.0 * boost::math::constants::pi<double>() / min_dist );
        vec_in_cpx[0] = cpx_t( c, 0.0 );
    }
    {
        const double x = vec_in[1] - translation;
        const double c = std::abs(x) * cos(x) / ( 2.0 * boost::math::constants::pi<double>() / min_dist );
        vec_in_cpx[1] = cpx_t( c, 0.0 );
    }

    // Apply fast fourier transform
    static std::vector<cpx_t> vec_out_cpx;
    vec_out_cpx.resize( vec_in.size() );
    _fft_forward.transform( &vec_in_cpx[0] , &vec_out_cpx[0] );

    // Create output
    return inverse_fft_cost_abst( std::real( vec_out_cpx[0] ), min_dist );
}

}
}
}
