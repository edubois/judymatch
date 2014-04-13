#ifndef _JM_CORE_RETHROW_HPP_
#define	_JM_CORE_RETHROW_HPP_

#include <tango-core/config/global.hpp>
#include <tango-core/exceptions.hpp>

#define RETHROW( Ex, UsrMsg, DevMsg )\
    std::string errDev = "Unknown error!";\
    if ( boost::get_error_info<tango::exception::dev>( Ex ) )\
    { errDev = boost::to_string( *boost::get_error_info<tango::exception::dev>( Ex ) ); }\
    BOOST_THROW_EXCEPTION( tango::exception::Failed() << tango::exception::user() + _tgr( UsrMsg ) << tango::exception::dev() + _tgr( DevMsg ) + errDev );


#endif
