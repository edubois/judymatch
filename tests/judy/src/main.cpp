#define BOOST_TEST_MODULE judy_array_test

#include <unittest.hpp>
#include <iostream>

#include <judy/JudyArray.hpp>
#include <set>

BOOST_AUTO_TEST_SUITE( judy_array_suite )

using namespace boost::unit_test;

BOOST_AUTO_TEST_CASE( insert_size_size_function )
{
    std::cout << "Testing insert function on judy array<std::size_t, std::size_t>" << std::endl;
    using namespace judy;

    typedef JudyArray<std::size_t, std::size_t> JudyArrayT;
    JudyArrayT judy_array( sizeof( std::size_t ) );
    BOOST_CHECK( judy_array.size() == 0 );
    BOOST_CHECK( judy_array.count( 1 ) == 0 );
    BOOST_CHECK( judy_array.begin() == judy_array.end() );

    std::size_t v0( 1111 );
    judy_array.insert( std::make_pair( 3, v0 ) );
    BOOST_CHECK( judy_array.size() == 1 );
    BOOST_CHECK( judy_array.count( 3 ) == 1 );
    BOOST_CHECK( judy_array.begin() != judy_array.end() );

    JudyArrayT::iterator it_item;

    std::size_t v1( 2222 );
    judy_array.insert( std::make_pair( 2, v1 ) );
    it_item = judy_array.find( 2 );
    BOOST_CHECK( it_item != judy_array.end() );
    BOOST_CHECK( it_item->second == v1 );
    BOOST_CHECK( judy_array.size() == 2 );
    BOOST_CHECK( judy_array.count( 2 ) == 1 );

    std::size_t v2( 3333 );
    judy_array.insert( std::make_pair( 1, v2 ) );
    BOOST_CHECK( judy_array.count( 1 ) == 1 );
    BOOST_CHECK( judy_array.size() == 3 );
    it_item = judy_array.find( 1 );
    BOOST_CHECK( it_item != judy_array.end() );
    BOOST_CHECK( it_item->second == v2 );

    std::cout << "Checking items in judy array: " << std::endl;
    std::set<std::size_t> to_check = { v0, v1, v2 };
    std::set<std::size_t> checked;
    for( const auto v: judy_array )
    {
        std::cout << v.first << " :: " << v.second << std::endl;
        BOOST_CHECK( to_check.find( v.second ) != to_check.end() );
        checked.insert( v.second );
    }
    BOOST_CHECK( to_check == checked );
}

BOOST_AUTO_TEST_CASE( insert_string_string_function )
{
    std::cout << "Testing insert function on judy array<std::string, std::string>" << std::endl;
    using namespace judy;

    typedef JudyArray<std::string, std::string> JudyArrayT;
    JudyArrayT judy_array( 256, 0 );
    BOOST_CHECK( judy_array.size() == 0 );
    BOOST_CHECK( judy_array.begin() == judy_array.end() );

    std::string v0( "djoo" );
    judy_array.insert( std::make_pair( std::string( "djoo" ), v0 ) );
    BOOST_CHECK( judy_array.size() == 1 );
    BOOST_CHECK( judy_array.begin() != judy_array.end() );

    std::string v1( "fooo" );
    judy_array.insert( std::make_pair( std::string( "fooo" ), v1 ) );

    BOOST_CHECK( judy_array.size() == 2 );
    BOOST_CHECK_EQUAL( judy_array.count( "fooo" ), 1 );
    BOOST_CHECK( judy_array.begin() != judy_array.end() );
    JudyArrayT::iterator it_item;
    it_item = judy_array.find( "fooo" );
    BOOST_CHECK( it_item != judy_array.end() );
    BOOST_CHECK( it_item->second == v1 );

    std::string v2( "barr" );
    judy_array.insert( std::make_pair( std::string( "barr" ), v2 ) );
    judy_array.insert( std::make_pair( std::string( "barr" ), v1 ) );
    BOOST_CHECK( judy_array.size() == 4 );
    BOOST_CHECK( judy_array.count( "barr" ) == 2 );
    it_item = judy_array.find( "barr" );
    BOOST_CHECK( it_item != judy_array.end() );
    BOOST_CHECK( it_item->second == v2 );
    ++it_item;
    BOOST_CHECK( it_item->second == v1 );

    std::cout << "Checking items in judy array: " << std::endl;
    std::set<std::string> to_check = { v0, v1, v2 };
    for( const auto v: judy_array )
    {
        std::cout << v.first << " :: " << v.second << std::endl;
        BOOST_CHECK( to_check.find( v.second ) != to_check.end() );
    }
}

BOOST_AUTO_TEST_CASE( reverse_iterator_function )
{
    std::cout << "Testing iterator-- on judy array<std::string, std::string>" << std::endl;
    using namespace judy;

    typedef JudyArray<std::string, std::string> JudyArrayT;
    JudyArrayT judy_array( 256, 0 );
    judy_array.insert( std::make_pair( std::string( "aaa" ), std::string( "a1" ) ) );
    judy_array.insert( std::make_pair( std::string( "aaa" ), std::string( "a2" ) ) );
    judy_array.insert( std::make_pair( std::string( "bbb" ), std::string( "b1" ) ) );
    judy_array.insert( std::make_pair( std::string( "ccc" ), std::string( "c1" ) ) );
    judy_array.insert( std::make_pair( std::string( "ddd" ), std::string( "d1" ) ) );

    typename JudyArrayT::iterator it = judy_array.end();
    --it;
    BOOST_CHECK( it->first == std::string( "ddd" ) );
    BOOST_CHECK( it->second == std::string( "d1" ) );
    --it;
    BOOST_CHECK( it->first == std::string( "ccc" ) );
    BOOST_CHECK( it->second == std::string( "c1" ) );
    --it;
    BOOST_CHECK( it->first == std::string( "bbb" ) );
    BOOST_CHECK( it->second == std::string( "b1" ) );
    --it;
    BOOST_CHECK( it->first == std::string( "aaa" ) );
    BOOST_CHECK( it->second == std::string( "a2" ) );
    --it;
    BOOST_CHECK( it->first == std::string( "aaa" ) );
    BOOST_CHECK( it->second == std::string( "a1" ) );
    BOOST_CHECK( it == judy_array.begin() );
}

BOOST_AUTO_TEST_CASE( forward_iterator_function )
{
    std::cout << "Testing iterator++ on judy array<std::string, std::string>" << std::endl;
    using namespace judy;

    typedef JudyArray<std::string, std::string> JudyArrayT;
    JudyArrayT judy_array( 256, 0 );
    judy_array.insert( std::make_pair( std::string( "aaa" ), std::string( "a1" ) ) );
    judy_array.insert( std::make_pair( std::string( "aaa" ), std::string( "a2" ) ) );
    judy_array.insert( std::make_pair( std::string( "bbb" ), std::string( "b1" ) ) );
    judy_array.insert( std::make_pair( std::string( "ccc" ), std::string( "c1" ) ) );
    judy_array.insert( std::make_pair( std::string( "ddd" ), std::string( "d1" ) ) );

    typename JudyArrayT::iterator it = judy_array.begin();
    BOOST_CHECK( it->first == std::string( "aaa" ) );
    BOOST_CHECK( it->second == std::string( "a1" ) );
    ++it;
    BOOST_CHECK( it->first == std::string( "aaa" ) );
    BOOST_CHECK( it->second == std::string( "a2" ) );
    ++it;
    BOOST_CHECK( it->first == std::string( "bbb" ) );
    BOOST_CHECK( it->second == std::string( "b1" ) );
    ++it;
    BOOST_CHECK( it->first == std::string( "ccc" ) );
    BOOST_CHECK( it->second == std::string( "c1" ) );
    ++it;
    BOOST_CHECK( it->first == std::string( "ddd" ) );
    BOOST_CHECK( it->second == std::string( "d1" ) );
    ++it;
    BOOST_CHECK( it == judy_array.end() );
}

BOOST_AUTO_TEST_SUITE_END()
