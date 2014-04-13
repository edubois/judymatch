#ifndef _JM_DCT_HPP_
#define	_JM_DCT_HPP_

#include <judymatch/core/vec_type.hpp>

namespace judymatch
{
namespace signal
{

/**
 * Compute the 1D dct of a given signal
 * @param vecIn given signal
 * @param vecOut output dct coefficients
 */
void dct( const judymatch::vec_t & vecIn, judymatch::vec_t & vecOut );

}
}

#endif
