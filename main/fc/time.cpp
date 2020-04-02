#include <fc/time.hpp>
#include <time.h>

namespace fc {

uint32_t clock::now()
{
   return (uint32_t)(::clock() * 1000 / CLOCKS_PER_SEC );
}

} // fc