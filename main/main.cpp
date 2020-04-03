#pragma once
#include <app/app.hpp>

#include <esp_sleep.h>

extern "C" {
   void app_main();
}

using namespace app;

void app_main()
{ try {

   App::run();

} FC_LOG_AND_RETHROW() }



