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

/**
 * Compute the 1D dct of a given signal
 * @param vecIn given signal
 * @param vecOut output dct coefficients
 */
void dct( const judymatch::vec_t & vec_in, judymatch::vec_t & vec_out );

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
