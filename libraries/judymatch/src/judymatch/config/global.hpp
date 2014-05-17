/* JudyMatcher fast pattern matching library
 * Copyright (C) 2014 Eloi Du Bois
 * eloi.du.bois@gmail.com
 *
 * Rework of an idea I had in 2009.
 * 
 * PLEASE PUT MY NAME IN YOUR SPECIAL THANKS IF YOU USE THIS LIB
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for  more details.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 */

#ifndef _JM_GLOBAL_HPP_
#define _JM_GLOBAL_HPP_

//#define JM_NO_COUT

////////////////////////////////////////////////////////////////////////////////
// System stuff
#include "windows/windows.h"
#include "translation.hpp"
#include "version.hpp"

#include <judymatch/core/exceptions.hpp>
#include <judymatch/core/judy_vector_ext.hpp>
#include <judymatch/config/config.hpp>

////////////////////////////////////////////////////////////////////////////////
// Assert needs to be everywhere
#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>
#include <boost/current_function.hpp>

////////////////////////////////////////////////////////////////////////////////
// compatibility problems...
#include <cstddef>
namespace std {
#ifdef _MSC_VER
	typedef SSIZE_T ssize_t;
#else
	typedef ::ssize_t ssize_t;
#endif
}


////////////////////////////////////////////////////////////////////////////////
// Define functions to display infos in the console
#include <iostream>

#ifndef JM_COUT

/**
 * @def   JM_INFOS
 * @brief informations : filename, line number, function name
 **/
 #define JM_INFOS  "file: " << __FILE__ << ",  line: " << __LINE__ << ::std::endl << "function: " << BOOST_CURRENT_FUNCTION

 #define JM_VAR( a )  # a << ": " << a
 #define JM_VAR2( a, b )  # a << ": " << a << ", " << # b << ": " << b
 #define JM_VAR3( a, b, c )  # a << ": " << a << ", " << # b << ": " << b << ", " << # c << ": " << c
 #define JM_VAR4( a, b, c, d )  # a << ": " << a << ", " << # b << ": " << b << ", " << # c << ": " << c << ", " << # d << ": " << d
 #define JM_VAR_ENDL( a )  # a << ":" << ::std::endl << a

#ifndef JM_NO_COUT
/**
 * @param[in] ... : all parameters with an operator << defined
 * @brief terminal display
 **/
 #define JM_COUT(... )  ::std::cout << __VA_ARGS__ << ::std::endl
 #define JM_CERR(... )  ::std::cerr << __VA_ARGS__ << ::std::endl

 #define JM_COUT_X( N, ... ) \
    for( unsigned int i = 0; i < N; ++i ) { ::std::cout << __VA_ARGS__; } \
    ::std::cout << ::std::endl

#else
 #define JM_COUT(...)
 #define JM_CERR(...)
 #define JM_COUT_X( N, ... )
#endif


 #define JM_COUT_VAR( a )  JM_COUT( JM_VAR( a ) )
 #define JM_COUT_VAR2( a, b )  JM_COUT( JM_VAR2( a, b ) )
 #define JM_COUT_VAR3( a, b, c )  JM_COUT( JM_VAR3( a, b, c ) )
 #define JM_COUT_VAR4( a, b, c, d )  JM_COUT( JM_VAR4( a, b, c, d ) )

/**
 * @brief terminal information display
 **/
 #define JM_COUT_INFOS JM_COUT( JM_INFOS )

/**
 * @param[in] ... : all parameters with an operator << defined
 * @brief terminal information display
 **/
 #define JM_COUT_WITHINFOS(... )  \
    JM_COUT( JM_INFOS << \
          ::std::endl << "\t" << __VA_ARGS__ )

 #define JM_COUT_WARNING(... )  \
    JM_CERR( "Warning:" << \
    ::std::endl << JM_INFOS << \
    ::std::endl << "\t" << __VA_ARGS__  )

 #define JM_COUT_ERROR(... )  \
    JM_CERR( "Error:" << \
    ::std::endl << JM_INFOS << \
    ::std::endl << "\t" << __VA_ARGS__  )

 #define JM_COUT_FATALERROR(... )  \
    JM_CERR( "Fatal error:" << \
    ::std::endl << JM_INFOS << \
    ::std::endl << "\t" << __VA_ARGS__  )

#endif

////////////////////////////////////////////////////////////////////////////////
// Some specifics things to debug or release version
#ifdef DEBUG
 #include "debug.hpp"
#else
 #include "release.hpp"
#endif

#include "logging.hpp"

////////////////////////////////////////////////////////////////////////////////
// JM_TCOUT* defines are used by developpers for temporary displays during development stages.
// They are removed in production mode.
#ifndef JM_PRODUCTION
        #define JM_TCOUT JM_COUT
        #define JM_TCOUT_X JM_COUT_X
        #define JM_TCOUT_VAR JM_COUT_VAR
        #define JM_TCOUT_VAR2 JM_COUT_VAR2
        #define JM_TCOUT_VAR3 JM_COUT_VAR3
        #define JM_TCOUT_VAR4 JM_COUT_VAR4
        #define JM_TCOUT_INFOS JM_COUT_INFOS
        #define JM_TCOUT_WITHINFOS JM_COUT_WITHINFOS
        #define JM_TCOUT_EXCEPTION JM_COUT_EXCEPTION
#else
        #define JM_TCOUT JM_COUT_DEBUG
        #define JM_TCOUT_X JM_COUT_X_DEBUG
        #define JM_TCOUT_VAR JM_COUT_VAR_DEBUG
        #define JM_TCOUT_VAR2 JM_COUT_VAR2_DEBUG
        #define JM_TCOUT_VAR3 JM_COUT_VAR3_DEBUG
        #define JM_TCOUT_VAR4 JM_COUT_VAR4_DEBUG
        #define JM_TCOUT_INFOS JM_COUT_INFOS_DEBUG
        #define JM_TCOUT_WITHINFOS JM_COUT_WITHINFOS_DEBUG
        #define JM_TCOUT_EXCEPTION JM_COUT_EXCEPTION_DEBUG
#endif

#endif //!_JM_GLOBAL_HPP_
