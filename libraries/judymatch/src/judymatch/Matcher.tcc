/* Copyright (C) 2012, 2013, 2014, 2015 Eloi DU BOIS - All Rights Reserved
 * The license for this file is available here:
 * https://github.com/edubois/judymatch/blob/master/LICENSE.txt
 */

namespace judymatch
{

template<class T, class H>
Matcher<T, H>::Matcher( const H & geohasher )
: 
#ifdef USE_JUDY_ARRAY_T
_database( geohasher.hash_size() )
, _geohasher( geohasher )
#else
_geohasher( geohasher )
#endif
{
}

template<class T, class H>
Matcher<T, H>::~Matcher()
{
}

}
