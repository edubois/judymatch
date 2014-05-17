#include "dct.hpp"

#include <judymatch/config/global.hpp>

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
void dct( const judymatch::vec_t & vec_in, judymatch::vec_t & vec_out )
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

}
}
}
