/* Copyright (C) 2012, 2013, 2014, 2015 Eloi DU BOIS - All Rights Reserved
 * The license for this file is available here:
 * https://github.com/edubois/judymatch/blob/master/LICENSE.txt
 */

#ifndef _JM_RELEASE_HPP_
#define _JM_RELEASE_HPP_

/*
 * In release mode, JM_COUT_*_DEBUG are disabled.
 */

/// @see JM_COUT
#define JM_COUT_DEBUG(... )
/// @see  JM_COUT_INFOS
#define JM_COUT_INFOS_DEBUG
/// @see  JM_COUT_INFOS
#define JM_COUT_WITHINFOS_DEBUG(... )
/// @see JM_IF_DEBUG
#define JM_IF_DEBUG(... )

#define JM_COUT_X_DEBUG( N, ... )
#define JM_COUT_VAR_DEBUG(... )
#define JM_COUT_VAR2_DEBUG(... )
#define JM_COUT_VAR3_DEBUG(... )
#define JM_COUT_VAR4_DEBUG(... )
#define JM_COUT_WITHINFOS_DEBUG(... )
#define JM_COUT_WARNING_DEBUG(... )
#define JM_COUT_ERROR_DEBUG(... )
#define JM_COUT_FATALERROR_DEBUG(... )
#define JM_COUT_EXCEPTION_DEBUG(... )

#endif //!_JM_RELEASE_HPP_
