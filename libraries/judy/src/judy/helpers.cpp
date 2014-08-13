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
