cmake_minimum_required(VERSION 2.8.11)

project( Example1 CXX )

# Set Example1 versions
set( EXAMPLE1_VERSION_MAJOR "1" )
set( EXAMPLE1_VERSION_MINOR "0" )
set( EXAMPLE1_VERSION_MICRO "0" )
set( EXAMPLE1_VERSION ${EXAMPLE1_VERSION_MAJOR}.${EXAMPLE1_VERSION_MINOR}.${EXAMPLE1_VERSION_MICRO} )

# Define Example1 versions
add_definitions( -DEXAMPLE1_VERSION_MAJOR=${EXAMPLE1_VERSION_MAJOR} )
add_definitions( -DEXAMPLE1_VERSION_MINOR=${EXAMPLE1_VERSION_MINOR} )
add_definitions( -DEXAMPLE1_VERSION_MICRO=${EXAMPLE1_VERSION_MICRO} )
add_definitions( -DEXAMPLE1_STATIC_BUILD )

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
