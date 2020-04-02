#pragma once
#include <app/app.hpp>

#include <esp_sleep.h>

extern "C" {
   void app_main();
}

using namespace app;

void app_main()
{ try {

   gpio_install_isr_service(0);
   esp_sleep_enable_ext0_wakeup(Pins::control_btn, HIGH);
   Tasks::startup.create();

} FC_LOG_AND_RETHROW() }



