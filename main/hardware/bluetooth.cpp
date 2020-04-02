#include <hardware/bluetooth.hpp>
#include <fc/utilities.hpp>
#include <fc/config.hpp>

#include <esp_bt_main.h>
#include <esp_bt_device.h>
#include <esp_bt_defs.h>
#include <esp_err.h>

namespace hw {

void bluetooth::init(const bluetooth_config& config)
{
   release_memory( config.mem_release_mode );
   enable_controller( config.controller_mode );
   init_stack();
   enable_stack();
   set_device_name( config.device_name );
   LOGM( "bt module initialized" );
}

void bluetooth::deinit()
{
   // TODO
}

void bluetooth::release_memory( esp_bt_mode_t mode )
{
   esp_err_t err;
   err = esp_bt_controller_mem_release( mode );
   FC_EXCEPTION( err, "bt controller memory could not be released" );
   LOGM( "bt controller memory released" )
}

void bluetooth::enable_controller( esp_bt_mode_t mode )
{
   if( esp_bt_controller_get_status() == ESP_BT_CONTROLLER_STATUS_ENABLED )
      return;

   esp_err_t err;
   esp_bt_controller_config_t config = BT_CONTROLLER_INIT_CONFIG_DEFAULT();

   if( esp_bt_controller_get_status() == ESP_BT_CONTROLLER_STATUS_IDLE )
   {
      err = esp_bt_controller_init(&config);
      FC_EXCEPTION( err, "bt controller config could not be initiated" );

      while( esp_bt_controller_get_status() != ESP_BT_CONTROLLER_STATUS_INITED );
   }

   err = esp_bt_controller_enable( mode );
   FC_EXCEPTION( err, "bt controller could not be enabled" );
   LOGM( "bt controller enabled" );
}

void bluetooth::disable_controller()
{
   esp_err_t err = ESP_FAIL;

   if( esp_bt_controller_get_status() == ESP_BT_CONTROLLER_STATUS_IDLE )
      return;

   if( esp_bt_controller_get_status() == ESP_BT_CONTROLLER_STATUS_ENABLED )
   {
      err = esp_bt_controller_disable();
      FC_EXCEPTION( err, "bt controller could not be disabled" );

      while( esp_bt_controller_get_status() == ESP_BT_CONTROLLER_STATUS_ENABLED );
   }

   if( esp_bt_controller_get_status() == ESP_BT_CONTROLLER_STATUS_INITED )
      return;

   // TODO NEVER COMES HERE
   FC_EXCEPTION( ESP_FAIL, "bt controller could not be disabled" );
   LOGM( "bt controller disabled" );
}

void bluetooth::init_stack()
{
   esp_err_t err;
   err = esp_bluedroid_init();
   FC_EXCEPTION( err, "bt stack could not be initialized" );
   LOGM( "bt stack initialized" );
}

void bluetooth::enable_stack()
{
   esp_err_t err;
   err = esp_bluedroid_enable();
   FC_EXCEPTION( err, "bt stack could not be enabled" );
   LOGM( "bt stack enabled" );
}

void bluetooth::set_device_name(const char* dn)
{
   esp_err_t err;
   err = esp_bt_dev_set_device_name( dn );
   FC_EXCEPTION( err, "bt device name could not be set" );
   LOGM( "bt device name set" );
}

void bluetooth::set_discovery_mode( bt_connection_mode cm, bt_discovery_mode sm )
{
   esp_err_t err;
   err = esp_bt_gap_set_scan_mode(
      (esp_bt_connection_mode_t)cm,
      (esp_bt_discovery_mode_t)sm
   );
   FC_EXCEPTION( err, "bt scan mode could not be set" );
   LOGMV( "bt scan mode set", sm );
}

void bluetooth::register_auth(esp_bt_gap_cb_t auth_cb)
{
   esp_err_t err( esp_bt_gap_register_callback( auth_cb ) );
   FC_EXCEPTION( err, "bt authentication cb could not be registered" );
}

}
