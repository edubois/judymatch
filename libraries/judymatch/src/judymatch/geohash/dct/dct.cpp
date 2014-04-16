#include "dct.hpp"

#include <judymatch/config/global.hpp>

#include <opencv2/core/core.hpp>

namespace judymatch
{
namespace geohash
{
namespace dct
{

using namespace boost::numeric;

void dct( const judymatch::vec_t & vec_in, judymatch::vec_t & vec_out )
{
    BOOST_ASSERT( vec_in.size() );
    BOOST_ASSERT( vec_out.size() );
    vec_out.resize( vec_in.size() );
    cv::Mat mIn( 1, vec_in.size(), CV_64FC1, const_cast<double*>( &vec_in[0] ) );
    cv::Mat mOut( 1, vec_out.size(), CV_64FC1, &vec_out[0] );
    cv::dct( mIn, mOut );
}

}
}
}
