/* Copyright (C) 2012, 2013, 2014, 2015 Eloi DU BOIS - All Rights Reserved
 * The license for this file is available here:
 * https://github.com/edubois/judymatch/blob/master/LICENSE.txt
 */

#ifndef _JM_DEBUG_HPP_
#define _JM_DEBUG_HPP_

// pre-processeur directives :
//	# : convert to string
//	## : concatenate (or before __VA_ARGS__ to indicate that it may be empty)
//	__FILE__ : filename
//	__LINE__ : line number
//	__FUNCTION__ : function declaration
//	__PRETTY_FUNCTION__ : function name
//	__DATE__ : "Mmm dd yyyy"
//	__TIME__ : "hh:mm:ss"

//_____________________________________________________________________________
// Macros to output on terminal only in debug mode

/// @see JM_COUT
#define JM_COUT_DEBUG JM_COUT

/// @see  JM_COUT_INFOS
#define JM_COUT_INFOS_DEBUG JM_COUT_INFOS

/// @see  JM_COUT_INFOS
#define JM_COUT_WITHINFOS_DEBUG JM_COUT_WITHINFOS

/// @see JM_IF_DEBUG
#define JM_IF_DEBUG(... )  __VA_ARGS__

#define JM_COUT_X_DEBUG JM_COUT_X
#define JM_COUT_VAR_DEBUG JM_COUT_VAR
#define JM_COUT_VAR2_DEBUG JM_COUT_VAR2
#define JM_COUT_VAR3_DEBUG JM_COUT_VAR3
#define JM_COUT_VAR4_DEBUG JM_COUT_VAR4
#define JM_COUT_INFOS_DEBUG JM_COUT_INFOS
#define JM_COUT_WITHINFOS_DEBUG JM_COUT_WITHINFOS
#define JM_COUT_WARNING_DEBUG JM_COUT_WARNING
#define JM_COUT_ERROR_DEBUG JM_COUT_ERROR
#define JM_COUT_FATALERROR_DEBUG JM_COUT_FATALERROR
#define JM_COUT_EXCEPTION_DEBUG JM_COUT_EXCEPTION

#endif

