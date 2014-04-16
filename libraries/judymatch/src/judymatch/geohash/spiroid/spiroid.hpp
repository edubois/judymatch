#ifndef _JM_SPIROID_FFT_HPP_
#define	_JM_SPIROID_FFT_HPP_

#include <judymatch/config/global.hpp>

#include <kissfft/kissfft.hh>
#include <boost/math/special_functions/pow.hpp>
#include <boost/math/constants/constants.hpp>

namespace judymatch
{
namespace geohash
{
namespace spiroid
{

inline double fft_constant()
{
    static const double sqrt2pi = sqrt(2.0 / boost::math::constants::pi<double>() );
    return sqrt2pi;
}

/**
 * @brief gives the value of cos(t)*abs(t) running in the fft space
 * @param t cosine fft coefficient
 * @param min_dist minimal distance between two points
 * @return a location dependant index
 */
inline double fft_cost_abst( const double t, double min_dist )
{
    static const double sqrt_2_d_pi = std::sqrt( 2.0 / boost::math::constants::pi<double>() );
    const double t_pow2 = boost::math::pow<2>( t );
    min_dist = 2.0 * boost::math::constants::pi<double>() / min_dist;

    return - sqrt_2_d_pi * ( t_pow2 + 1.0 ) / ( min_dist * ( t_pow2 - 2.0 ) * t_pow2 + min_dist );
}

/**
 * @brief compute inverse of fft_cost_abst
 * @param x
 * @return the linearized value
 */
inline double inverse_fft_cost_abst( const double t, double min_dist )
{
    static const double sqrt_2_d_pi = std::sqrt(2.0 / boost::math::constants::pi<double>() );
    const double t_pow2 = boost::math::pow<2>( t );
    min_dist = 2.0 * boost::math::constants::pi<double>() / min_dist;

    return -sqrt_2_d_pi * ( t_pow2 + 1 ) / ( min_dist * ( ( t_pow2 * ( t_pow2 - 2 ) ) + 1 ) );
}

class spiroid_geohasher
{
public:
    typedef double value_type;

private:
    typedef kissfft<vec_t::value_type> fft_t;
public:
    /**
     * @brief constructor
     * @param vec_size
     */
    spiroid_geohasher( const std::size_t vec_size, const double min_dist )
    : _fft_forward( vec_size, false )
    , _min_dist( min_dist )
    {
    }

    /**
     * @brief Compute the 1D location dependant spiroid fft of a given signal (@patent Eloi du Bois)
     * @param fft fft engine
     * @param vec_in given signal
     * @param min_dist minimal space between two points
     * @return an geospatial hash index
     */
    double loc_spiroid_fft( const judymatch::vec_t & vec_in, const double min_dist, const double translation ) const;

    const value_type hash_for_construction( const vec_t & vec_in ) const
    {
        return loc_spiroid_fft( vec_in, _min_dist, 0.0 );
    }

    const value_type hash_for_find_back( const vec_t & vec_in ) const
    {
        return loc_spiroid_fft( vec_in, _min_dist, 0.0 );
    }

    inline static const std::size_t hash_size()
    { return sizeof( value_type ); }

private:
    mutable fft_t _fft_forward;
    double _min_dist;
};

}
}
}

#endif
