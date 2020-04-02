#pragma once

/**
 *  nvs_flash is used to store PHY (physical layer) calibration data
 *  nvs_flash is a singleton and can only be accessed through nvs_flash::instance()
 *  nvs_flash::init() is the first function to be called in main
 */

namespace hw {

class nvs_flash
{
public:
   static void init();

private:
   nvs_flash() = delete;
};

}
