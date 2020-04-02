#pragma once
#include <esp_bt.h>
#include <esp_gap_bt_api.h>

namespace hw {

enum bt_discovery_mode
{
   discoverable     = ESP_BT_GENERAL_DISCOVERABLE,
   non_discoverable = ESP_BT_NON_DISCOVERABLE
};

enum bt_connection_mode
{
   connectable     = ESP_BT_CONNECTABLE,
   non_connectable = ESP_BT_NON_CONNECTABLE
};

struct bluetooth_config
{
public:
   const char* device_name;
   esp_bt_mode_t controller_mode;
   esp_bt_mode_t mem_release_mode;
};

class bluetooth
{
public:
   static void init(const bluetooth_config& config);
   static void deinit();
   static void set_discovery_mode(bt_connection_mode cm, bt_discovery_mode sm);
   static void register_auth(esp_bt_gap_cb_t auth_b);

   static void release_memory(esp_bt_mode_t);
   static void enable_controller(esp_bt_mode_t);
   static void disable_controller();
   static void init_stack();
   static void enable_stack();
   static void set_device_name(const char* name);

private:
   bluetooth() = delete;
};

}
