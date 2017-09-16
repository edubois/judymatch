cmake_minimum_required(VERSION 2.8.11)

project( JudyTest CXX )

# Set JudyTest versions
set( JUDY_TEST_VERSION_MAJOR "1" )
set( JUDY_TEST_VERSION_MINOR "0" )
set( JUDY_TEST_VERSION_MICRO "0" )
set( JUDY_TEST_VERSION ${JUDY_TEST_VERSION_MAJOR}.${JUDY_TEST_VERSION_MINOR}.${JUDY_TEST_VERSION_MICRO} )

# Define JudyTest versions
add_definitions( -DJUDY_TEST_VERSION_MAJOR=${JUDY_TEST_VERSION_MAJOR} )
add_definitions( -DJUDY_TEST_VERSION_MINOR=${JUDY_TEST_VERSION_MINOR} )
add_definitions( -DJUDY_TEST_VERSION_MICRO=${JUDY_TEST_VERSION_MICRO} )
add_definitions( -DJUDY_TEST_STATIC_BUILD )

# Diplay commands being ran by CMake
set( CMAKE_VERBOSE_MAKEFILE OFF )

# CPP flags on debug / release mode
if( MSVC )
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
else()
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -fPIC -pg -g")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fPIC -O3")
endif()

add_subdirectory(src)
