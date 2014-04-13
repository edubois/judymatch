#include "dct.hpp"

#include <judymatch/config/global.hpp>

#include <opencv2/core/core.hpp>

namespace judymatch
{
namespace signal
{
using namespace boost::numeric;

void dct( const ublas::vector<RealT> & vecIn, ublas::vector<RealT> & vecOut )
{
    BOOST_ASSERT( vecIn.size() );
    vecOut.resize( vecIn.size() );
    cv::Mat mIn( 1, vecIn.size(), CV_64FC1, const_cast<double*>( &vecIn[0] ) );
    cv::Mat mOut( 1, vecOut.size(), CV_64FC1, &vecOut[0] );
    cv::dct( mIn, mOut );
}

}
}
