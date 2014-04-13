#ifndef _JM_DCT_HPP_
#define	_JM_DCT_HPP_

#include <judymatch/config/config.hpp>

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>

namespace judymatch
{
namespace signal
{

/**
 * Compute the 1D dct of a given signal
 * @param vecIn given signal
 * @param vecOut output dct coefficients
 */
void dct( const boost::numeric::ublas::vector<RealT> & vecIn, boost::numeric::ublas::vector<RealT> & vecOut );

}
}

#endif
