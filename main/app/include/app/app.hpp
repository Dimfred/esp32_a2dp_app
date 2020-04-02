#pragma once
#include <freertospp/freertospp.hpp>

#include <hardware/nvs_flash.hpp>
#include <hardware/bluetooth.hpp>
#include <hardware/a2dp.hpp>
#include <hardware/i2s.hpp>
#include <hardware/pin.hpp>

#include <fc/utilities.hpp>
#include <fc/exception.hpp>
#include <fc/config.hpp>

namespace app {

struct Pins
{
   static hw::pin control_btn;
   static hw::pin show_battery_btn;
   static hw::pin show_battery_display;
   static hw::pin vibration;
   static hw::pin status_led;
};

struct Timers
{
   static freertospp::Timer startup_shutdown;
   static freertospp::Timer shutdown_not_connected;
};

struct Tasks
{
   static freertospp::Task init_app;
   static freertospp::Task show_battery;
   static freertospp::Task startup;
   static freertospp::Task shutdown;
   static freertospp::Task status_blink;
};

struct A2dp
{
   static void on_data( const uint8_t *data, uint32_t len );
   static void on_connected(const hw::a2dp::connection_state&);
   static void on_disconnected(const hw::a2dp::connection_state&);
   static void on_audio_configure(const hw::a2dp::audio_config&);
};

struct Bluetooth
{
   static void authentication(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param);
};

}