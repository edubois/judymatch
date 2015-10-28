/* Copyright (C) 2012, 2013, 2014, 2015 Eloi DU BOIS - All Rights Reserved
 * The license for this file is available here:
 * https://github.com/edubois/judymatch/blob/master/LICENSE.txt
 */

#ifndef _JM_CORE_RETHROW_HPP_
#define	_JM_CORE_RETHROW_HPP_

#include <judymatch/config/global.hpp>
#include <judymatch/exceptions.hpp>

#define RETHROW( Ex, UsrMsg, DevMsg )\
    std::string errDev = "Unknown error!";\
    if ( boost::get_error_info<judymatch::exception::dev>( Ex ) )\
    { errDev = boost::to_string( *boost::get_error_info<judymatch::exception::dev>( Ex ) ); }\
    BOOST_THROW_EXCEPTION( judymatch::exception::Failed() << judymatch::exception::user() + _tgr( UsrMsg ) << judymatch::exception::dev() + _tgr( DevMsg ) + errDev );


#endif
