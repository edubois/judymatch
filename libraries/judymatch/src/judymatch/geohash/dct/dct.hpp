#ifndef _JM_DCT_HPP_
#define	_JM_DCT_HPP_

#include <judymatch/core/vec_type.hpp>

namespace judymatch
{
namespace geohash
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
class dct_1d_geohasher
{
public:
    typedef judymatch::vec_t value_type;

public:
    const vec_t & operator()( const vec_t & vec_in )
    {
        static vec_t vec_out;
        vec_out.resize( vec_in.size() );
        dct( vec_in, vec_out );
        return vec_out;
    }

    inline static const std::size_t hash_size()
    { return S * sizeof( value_type ); }
};

}
}
}

#endif
