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
