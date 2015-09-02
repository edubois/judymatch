#ifndef _JM_POLAR_LSH_HPP_
#define	_JM_POLAR_LSH_HPP_

#include <judymatch/config/global.hpp>
#include <judymatch/core/vec_type.hpp>

#include <boost/math/special_functions/pow.hpp>
#include <boost/math/constants/constants.hpp>

namespace judymatch
{
namespace lsh
{
namespace polar
{

using namespace boost::numeric;
static const double kPolarScaleUpFactor( 100000000.0 );

/**
 * @brief compute polar value of x and y
 */
inline void polar_xy( const double x, const double y, double & theta, double & d)
{
    theta = std::atan2( y, x );
    d = std::sqrt( boost::math::pow<2>( x ) + boost::math::pow<2>( y ) );
}

/**
 * @brief compute polar value of x and y and pack it into a double value
 */
inline double polar_real_packed( const double x, const double y )
{
    double theta; // Theta
    double d;     // Distance
    polar_xy( x, y, theta, d );
    // We want to make a unique double from two doubles
    return std::round( std::log( theta + boost::math::constants::pi<double>() + 1.0 ) * kPolarScaleUpFactor ) + ( 1.0 / std::log( d + 1.0 ) );
}

/**
 * Compute the 1D dct of a given signal (only working on vec of size() == 2 for the moment)
 * @param vecIn given signal
 * @param vecOut output dct coefficients
 */
template<class V>
void polar_lsh( const V & vec_in, double & result )
{
    using namespace boost::math;
    using namespace boost::math::constants;
    BOOST_ASSERT( vec_in.size() );
    result = vec_in[0];
    for( std::size_t i = 1; i < vec_in.size(); ++i )
    {
        result = polar_real_packed( result, vec_in[i] );
    }
}

/**
 * Compute the 1D dct of a given string
 * @param str_in given string
 * @param str_out output dct coefficients shrinked into string chars
 */
template<class T>
void polar_lsh_string( const T & str_in, T & str_out )
{
    using namespace boost::math;
    using namespace boost::math::constants;

    double polar_overpacked = 0.0;
    polar_lsh( str_in, polar_overpacked );
    const char *doubleData = reinterpret_cast<const char*>( &polar_overpacked );
    for( std::size_t k = 0; k < sizeof(double) * 2; ++k )
    {
        str_out += doubleData[k];
    }
}

template<class T>
class polar_1d_lsh_string
{
public:
    typedef T value_type;

public:
    polar_1d_lsh_string() {}

    /**
     * @brief Compute the 1D location dependant lsh of a given signal
     * @param vec_in given signal
     * @return an lsh (index giving an order relation)
     */
    const value_type hash_for_construction( const value_type & vec_in ) const
    {
        value_type vec_out;
        polar_lsh_string( vec_in, vec_out );
        return vec_out;
    }

    /**
     * @brief Compute the 1D location independant lsh of a given string
     * @param vec_in given string
     * @return an lsh (index giving an order relation)
     */
    const value_type hash_for_find_back( const value_type & vec_in ) const
    { return hash_for_construction( vec_in ); }

    inline static const std::size_t hash_size()
    { return 0; } // Dynamic
};

template<std::size_t S>
class polar_1d_lsh
{
public:
    typedef judymatch::vec_t value_type;

public:
    polar_1d_lsh() {}

    /**
     * @brief Compute the 1D location dependant lsh of a given signal
     * @param vec_in given signal
     * @return an lsh (index giving an order relation)
     */
    const value_type & hash_for_construction( const vec_t & vec_in ) const
    {
        static vec_t vec_out;
        if ( vec_out.size() != 1 )
        {
            vec_out.resize( 1 );
        }
        polar_lsh( vec_in, vec_out[0] );
        return vec_out;
    }

    /**
     * @brief Compute the 1D location independant lsh of a given signal
     * @param vec_in given signal
     * @return an lsh (index giving an order relation)
     */
    const value_type & hash_for_find_back( const vec_t & vec_in ) const
    { return hash_for_construction( vec_in ); }

    inline static const std::size_t hash_size()
    { return S * sizeof( typename value_type::value_type ); }
};

}
}
}

#endif
