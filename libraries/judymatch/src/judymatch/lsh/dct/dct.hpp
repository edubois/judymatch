#ifndef _JM_DCT_HPP_
#define	_JM_DCT_HPP_

#include <judymatch/config/global.hpp>
#include <judymatch/core/vec_type.hpp>

#include <boost/math/special_functions/pow.hpp>
#include <boost/math/constants/constants.hpp>

namespace judymatch
{
namespace lsh
{
namespace dct
{

using namespace boost::numeric;

/**
 * Compute the 1D dct of a given signal
 * @param vecIn given signal
 * @param vecOut output dct coefficients
 */
template<class V>
inline void dct( const V & vec_in, V & vec_out )
{
    using namespace boost::math;
    using namespace boost::math::constants;
    BOOST_ASSERT( vec_in.size() );
    BOOST_ASSERT( vec_out.size() );
    const double k_pi_vec_sz = pi<double>() / ( vec_in.size() );

    for( std::size_t i = 0; i < vec_in.size(); ++i )
    {
        vec_out[i] = 0.0;
        for( std::size_t j = 0; j < vec_in.size(); ++j )
        {
            vec_out[i] += vec_in[j] * std::cos( k_pi_vec_sz * ( j + 0.5 ) * ( i ) );
        }
    }
}

/**
 * Compute the 1D dct of a given string
 * @param str_in given string
 * @param str_out output dct coefficients shrinked into string chars
 */
template<class T>
inline void dct_string( const T & str_in, T & str_out )
{
    using namespace boost::math;
    using namespace boost::math::constants;
    const double k_pi_vec_sz = pi<double>() / ( str_in.length() );

    for( std::size_t i = 0; i < str_in.length(); ++i )
    {
        double v = 0;
        for( std::size_t j = 0; j < str_in.length(); ++j )
        {
            v += str_in[j] * std::cos( k_pi_vec_sz * ( j + 0.5 ) * ( i ) );
        }
        const char *doubleData = reinterpret_cast<const char*>( &v );
        for( std::size_t k = 0; k < sizeof(double); ++k )
        {
            str_out += doubleData[k];
        }
    }
}

template<class T>
class dct_1d_lsh_string
{
public:
    typedef T value_type;

public:
    dct_1d_lsh_string() {}

    /**
     * @brief Compute the 1D location dependant lsh of a given signal
     * @param vec_in given signal
     * @return an lsh (index giving an order relation)
     */
    const value_type hash_for_construction( const value_type & vec_in ) const
    {
        value_type vec_out;
        dct_string( vec_in, vec_out );
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
class dct_1d_lsh
{
public:
    typedef judymatch::vec_t value_type;

public:
    dct_1d_lsh() {}

    /**
     * @brief Compute the 1D location dependant lsh of a given signal
     * @param vec_in given signal
     * @return an lsh (index giving an order relation)
     */
    const value_type & hash_for_construction( const vec_t & vec_in ) const
    {
        static vec_t vec_out;
        if ( vec_out.size() != vec_in.size() )
        {
            vec_out.resize( vec_in.size() );
        }
        dct( vec_in, vec_out );
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
