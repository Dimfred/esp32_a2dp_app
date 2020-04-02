#include <freertospp/freertospp.hpp>

namespace freertospp {

uint16_t Timer::_timer_counter = 0;

uint32_t ms_to_ticks(uint32_t ms)
{
   return ms / portTICK_RATE_MS;
}

void delay(uint32_t ms)
{
   vTaskDelay(ms_to_ticks(ms));
}

}