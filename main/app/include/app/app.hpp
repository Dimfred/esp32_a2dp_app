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

class App
{
public:
   enum state {
      wakeup,
      startapp,
      disconnected,
      connected,
      shutdown,
   };

   static void run();
   static void apply_next_state(state state);

private:
   static state next_state;
   static state prev_state;
   static freertospp::Task task;
};

class Wakeup
{
public:
   static constexpr uint16_t HOLD_BUTTON_FOR_MS = 2000;
   static constexpr uint16_t TASK_PERIOD_MS     = 500;
   static constexpr uint16_t SHUTDOWN_AFTER_MS  = 3 * TASK_PERIOD_MS;

   static freertospp::Task task;
private:
};

struct Startapp
{
   static freertospp::Task init_app;
};

struct Connected
{
};

struct Disconnected
{
   static constexpr uint32_t SHUTDOWN_AFTER_S = 180;
   static constexpr uint32_t SHUTDOWN_TIMER_PERIOD_MS = 10000;
   static uint32_t shutdown_time_passed_ms;
   static freertospp::Timer shutdown_timer;

   static constexpr uint16_t BLINK_PERIOD_MS = 600;
   static freertospp::Task status_blink_task;
};

struct Common
{
   static constexpr uint16_t VIBRATION_TIME = 1000;
   static freertospp::Task vibration_task;

   static constexpr uint16_t SHOW_BATTERY_FOR_MS = 1500;
   static freertospp::Task show_battery_task;


   static constexpr uint16_t SHUTDOWN_AFTER_MS = 2500;
   static constexpr uint16_t SHUTDOWN_TASK_PERIOD_MS = 500;
   static freertospp::Task shutdown_task;
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