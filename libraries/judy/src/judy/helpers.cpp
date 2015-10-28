/* Copyright (C) 2012, 2013, 2014, 2015 Eloi DU BOIS - All Rights Reserved
 * The license for this file is available here:
 * https://github.com/edubois/judymatch/blob/master/LICENSE.txt
 */

#include "helpers.hpp"

namespace judy
{

template<>
void clear_buffer<std::string>( unsigned char * ptr, const std::size_t len )
{
    assert( len > 0 );
    ptr[0] = '\0';
}

}
