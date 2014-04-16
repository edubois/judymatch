namespace judymatch
{

template<class T, class H>
Matcher<T, H>::Matcher( const H & geohasher )
: _database( geohasher.hash_size() )
, _geohasher( geohasher )
{
}

template<class T, class H>
Matcher<T, H>::~Matcher()
{
}

}
