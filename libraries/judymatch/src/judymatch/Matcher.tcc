namespace judymatch
{

template<class T, class H>
Matcher<T, H>::Matcher( const H & geohasher )
: _geohasher( geohasher )
{
}

template<class T, class H>
Matcher<T, H>::~Matcher()
{
}

}
