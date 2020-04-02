#include <hardware/nvs_flash.hpp>
#include <fc/exception.hpp>

#include <nvs_flash.h>
#include <esp_system.h>


namespace hw {

void nvs_flash::init()
{
   esp_err_t err = nvs_flash_init();
   if( err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND ) {
      nvs_flash_erase();
      err = nvs_flash_init();
      FC_EXCEPTION( err, "error in initialization of nvs_flash" );
   }
}

}