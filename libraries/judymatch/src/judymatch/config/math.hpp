/* Copyright (C) 2012, 2013, 2014, 2015 Eloi DU BOIS - All Rights Reserved
 * The license for this file is available here:
 * https://github.com/edubois/judymatch/blob/master/LICENSE.txt
 */

#ifndef JM_MATH_HPP
#define JM_MATH_HPP

namespace js {

using std::min;
using std::max;

template<typename T>
inline T min( const T& a, const T& b, const T& c )
{
	return min( min( a, b ), c );
}

template<typename T>
inline T max( const T& a, const T& b, const T& c )
{
	return max( max( a, b ), c );
}

template<typename T>
inline T min( const T& a, const T& b, const T& c, const T& d )
{
	return min( min( a, b ), min( c, d ) );
}

template<typename T>
inline T max( const T& a, const T& b, const T& c, const T& d )
{
	return max( max( a, b ), max( c, d ) );
}

}

#endif

