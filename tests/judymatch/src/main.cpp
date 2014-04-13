#define BOOST_TEST_MODULE judy_match_test

#include <unittest.hpp>
#include <iostream>

#include <judy/JudyArray.hpp>
#include <set>

BOOST_AUTO_TEST_SUITE( judy_match_suite )

using namespace boost::unit_test;

BOOST_AUTO_TEST_CASE( find_closest_existing_function )
{
    std::cout << "Testing find closest existing element function on judy array<vec_t, vec_t>" << std::endl;
    using namespace judy;

    typedef JudyArray<std::size_t, std::size_t> JudyArrayT;
    JudyArrayT judy_array( sizeof( std::size_t ) );
}

BOOST_AUTO_TEST_SUITE_END()
