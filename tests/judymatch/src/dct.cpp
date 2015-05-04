#define BOOST_TEST_MODULE judy_match_test

#include <unittest.hpp>
#include <iostream>

#include <judymatch/config/global.hpp>
#include <judymatch/Matcher.hpp>
#include <judymatch/geometry/distance.hpp>
#include <judymatch/lsh/dct/dct.hpp>
#include <judymatch/lsh/polar/polar.hpp>

#include <boost/random.hpp>
#include <boost/foreach.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

BOOST_AUTO_TEST_SUITE( judy_match_suite )

using namespace boost::unit_test;

static const std::size_t k_data_length = 2;
static const std::size_t k_database_length( 1024 * 1024 * 8 );
static const std::size_t k_window_search = 1;  ///< Needed because some LSH functions are approximative

static const std::size_t k_threshold_validation = 5.0;          ///< Thershold for accepting results, put 0 for exactness

static const double kNoiseSigmaDatabase = 20.0;
static const double kNoiseSigmaIncomingVec = 20.0;

typedef judymatch::lsh::polar::polar_1d_lsh<k_data_length> loc_sens_hasher;

inline void fill_sequence_vector( const double num_seq, judymatch::vec_t & vec_out )
{
    for( std::size_t k = 0; k < vec_out.size(); ++k )
    {
        vec_out[ k ] = num_seq;
    }
}

inline void fill_random_vector( judymatch::vec_t & vec_out )
{
    // Random generator number generator
	static boost::mt19937 seed( time( NULL ) );
	static boost::uniform_real<> dist( 0.0, 256.0 );
	static boost::variate_generator<boost::mt19937&, boost::uniform_real<> > rand_gen( seed, dist );

    for( std::size_t k = 0; k < vec_out.size(); ++k )
    {
        vec_out[ k ] = std::abs( rand_gen() );
    }
}

inline void noise_vector( judymatch::vec_t & vec_out, const double sigma = 1.0, const double mean = 0.0 )
{
    // Random generator number generator
	static boost::mt19937 seed( time( NULL ) );
	static boost::normal_distribution<judymatch::vec_t::value_type> dist( mean, sigma );
	static boost::variate_generator<boost::mt19937&, boost::normal_distribution<double> > rand_gen( seed, dist );

    for( std::size_t k = 0; k < vec_out.size(); ++k )
    {
        vec_out[ k ] += rand_gen();
        vec_out[ k ] = std::abs( vec_out[ k ] );
    }
}

BOOST_AUTO_TEST_CASE( find_closest_0_0_function )
{
    JM_COUT( ">>>>> Testing find neighborhood of (0, 0) <<<<<<" );
    using namespace judymatch;
    typedef Matcher<vec_t, loc_sens_hasher> matcher_t;

    JM_COUT( "Generating a database with size: " << k_database_length << " vectors of dimension " << k_data_length << " (" << k_database_length * k_data_length << "Bytes)" );

    matcher_t matcher( ( loc_sens_hasher() ) );
    std::vector<vec_t> raw_database;
    raw_database.reserve( k_database_length );
//    matcher.database().reserve( k_database_length );

    // Generate a random database
    {
        vec_t vec( k_data_length );
        for( std::size_t i = 0; i < k_database_length; ++i )
        {
            fill_sequence_vector( i, vec );
            noise_vector( vec, kNoiseSigmaDatabase );
            matcher.insert( vec );
            raw_database.push_back( vec );
        }
    }

    BOOST_CHECK( matcher.size() == k_database_length );

    vec_t vec_to_find( k_data_length );
    fill_sequence_vector( 0.0, vec_to_find );
    matcher_t::iterator it_closest;
#ifdef USE_JUDY_ARRAY_T
    it_closest = matcher.closest( vec_to_find );
#else
    it_closest = matcher.closest_precise( vec_to_find, k_window_search );
#endif
    if ( it_closest == matcher.begin() )
    {
        JM_COUT( "Trivial result: the closest item is the first one in the judy array..." );
    }
    vec_t should_be_closest = it_closest->second;
    JM_COUT_VAR( should_be_closest );
    const double d_should_be_closest = geometry::euclidean_distance( should_be_closest, vec_to_find );
    double min_dist = std::numeric_limits<double>::max();
    vec_t raw_closest;
    {
        for( const vec_t & vec: raw_database )
        {
            const double d = geometry::euclidean_distance( vec, vec_to_find );
            if ( d < min_dist )
            {
                raw_closest = vec;
                min_dist = d;
            }
        }
    }

    const double d_sol = geometry::euclidean_distance( raw_closest, should_be_closest );
    std::cout << "Distance to correctness: " << d_sol << std::endl;
    if ( d_sol <= k_threshold_validation )
    {
        JM_COUT( "Test PASSED..." );
    }
    else
    {
        JM_COUT( "Test FAILED..." );
    }
    BOOST_CHECK( d_sol <= k_threshold_validation );
}


BOOST_AUTO_TEST_CASE( find_closest_random_function )
{
    JM_COUT( ">>>>> Testing find neighborhood of a random vector <<<<<" );
    using namespace judymatch;
    typedef Matcher<vec_t, loc_sens_hasher > matcher_t;

    matcher_t matcher( ( loc_sens_hasher() ) );
    std::vector<vec_t> raw_database;
    JM_COUT( "Generating a database with size: " << k_database_length << " vectors of dimension " << k_data_length << " (" << k_database_length * k_data_length << "Bytes)" );
    raw_database.reserve( k_database_length );
//    matcher.database().reserve( k_database_length );

    // Generate a random database
    {
        JM_COUT( "Filling database..." );
        boost::posix_time::ptime tstart( boost::posix_time::microsec_clock::local_time() );
        vec_t vec( k_data_length );
        for( std::size_t i = 0; i < k_database_length; ++i )
        {
            fill_sequence_vector( i, vec );
            noise_vector( vec, kNoiseSigmaDatabase );
            matcher.insert( vec );
            raw_database.push_back( vec );
        }

        boost::posix_time::ptime tstop( boost::posix_time::microsec_clock::local_time() );
        boost::posix_time::time_duration d = tstop - tstart;
        const double spendTime = d.total_milliseconds();
        JM_COUT( "Database filling took: " << spendTime << "ms." );
    }

    BOOST_CHECK( matcher.size() == k_database_length );

    JM_COUT( "Find closest element..." );
    vec_t vec_to_find( k_data_length );
    fill_sequence_vector( k_data_length/2, vec_to_find );
    noise_vector( vec_to_find, kNoiseSigmaIncomingVec );
    JM_COUT( "Input vector is: " << vec_to_find );
    matcher_t::iterator it_closest;
    {
        boost::posix_time::ptime tstart( boost::posix_time::microsec_clock::local_time() );
#ifdef USE_JUDY_ARRAY_T
        it_closest = matcher.closest( vec_to_find );
#else
        it_closest = matcher.closest_precise( vec_to_find, k_window_search );
#endif
        boost::posix_time::ptime tstop( boost::posix_time::microsec_clock::local_time() );
        boost::posix_time::time_duration d = tstop - tstart;
        const double spendTime = d.total_microseconds();
        JM_COUT( "Finding closest element with LSH algorithm took: " << spendTime << "microsec." );
    }
    vec_t should_be_closest = it_closest->second;
    const double d_should_be_closest = geometry::euclidean_distance( should_be_closest, vec_to_find );
    if ( it_closest != matcher.end() )
    {
        JM_COUT( "Closest candidate is: " << should_be_closest );
    }
    else
    {
        JM_COUT( "Item not found!" );
    }
    
    JM_COUT( "Now checking the results with a bruteforce algorithm..." );
    double min_dist = std::numeric_limits<double>::max();
    vec_t raw_closest;
    {
        boost::posix_time::ptime tstart( boost::posix_time::microsec_clock::local_time() );
        for( const vec_t & vec: raw_database )
        {
            const double d = geometry::euclidean_distance( vec, vec_to_find );
            if ( d < min_dist )
            {
                raw_closest = vec;
                min_dist = d;
            }
        }

        boost::posix_time::ptime tstop( boost::posix_time::microsec_clock::local_time() );
        boost::posix_time::time_duration d = tstop - tstart;
        JM_COUT( "Finding closest element with brute force algorithm took: " << d.total_milliseconds() << "ms (" << d.total_microseconds() << " microsecs.)" );
    }
    JM_COUT( "RAW search found: " << raw_closest );

    const double d_sol = geometry::euclidean_distance( raw_closest, should_be_closest );
    if ( d_sol <= k_threshold_validation )
    {
        JM_COUT( "Test PASSED..." );
    }
    else
    {
        JM_COUT( "Test FAILED..." );
        JM_CERR( "ALGORITHM found another item (might be an approximation)." );
        JM_COUT( "Distance between raw_closest and should_be_closest: " << d_sol );
        JM_COUT( "" );
        JM_COUT( "Distance between should_be_closest and vec_to_find: " << d_should_be_closest );
        JM_COUT( "Distance between raw_closest and vec_to_find: " 
                  << geometry::euclidean_distance( raw_closest, vec_to_find ) );
    }
    BOOST_CHECK( d_sol <= k_threshold_validation );
}

BOOST_AUTO_TEST_SUITE_END()
