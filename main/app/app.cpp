#include <app/app.hpp>

#include <esp_sleep.h>
#include <esp_spp_api.h>

using namespace freertospp;
using namespace hw;

namespace app {

pin Pins::control_btn(CONFIG::PINS::ON_OFF_BTN, INPUT);
pin Pins::show_battery_btn(CONFIG::PINS::SHOW_BATTERY_BTN, INPUT);
pin Pins::show_battery_display(CONFIG::PINS::SHOW_BATTERY_DISPLAY, OUTPUT);
pin Pins::vibration(CONFIG::PINS::VIBRATION, OUTPUT);
pin Pins::status_led(CONFIG::PINS::STATUS_LED, OUTPUT);

Timer Timers::startup_shutdown(ms_to_ticks(1000), false, "startup_shutdown_timer",
   +[](TimerHandle_t self)
   {
      LOGM("deep sleep activated");
      esp_deep_sleep_start();
   }
);

/// The startup task is the first one to be started. It creates a timer which will
/// put the esp into deepsleep if the user doesn't hold a button for some seconds.
/// If the condition is met the startup task kills itself and initializes the app
Task Tasks::startup(20, 2048, nullptr, "startup_task",
   +[](void* arg)
   {
      LOGM("startup_task started");

      constexpr uint16_t STARTUP_AFTER_MS = 2400;
      constexpr uint16_t PERIOD_MS        = 300;
      constexpr uint16_t MAX_COUNTER      = STARTUP_AFTER_MS / PERIOD_MS;
      constexpr uint16_t VIBRATION_START  = MAX_COUNTER - 2;
      constexpr uint16_t VIBRATION_UNTIL  = MAX_COUNTER - 1;

      Timers::startup_shutdown.create();
      Timers::startup_shutdown.start();

      uint16_t counter = 0;
      for(;;)
      {
         switch(counter)
         {
         case MAX_COUNTER:
            Timers::startup_shutdown.destroy();
            Pins::vibration.write(LOW);
            Tasks::init_app.create();
            Tasks::startup.destroy();
            break;
         case VIBRATION_START...VIBRATION_UNTIL:
            Pins::vibration.write(HIGH);
            break;
         default:
            Pins::vibration.write(LOW);
            break;
         }

         if( Pins::control_btn.read() )
         {
            ++counter;
            Timers::startup_shutdown += PERIOD_MS;
         }
         else {
            counter = 0;
         }

         delay(PERIOD_MS);
      }
   }
);

Task Tasks::init_app(21, 4096, nullptr, "init_app_task",
   +[](void* arg)
   {
      Timers::shutdown_not_connected.create();
      Timers::shutdown_not_connected.start();

      nvs_flash::init();

      bluetooth_config bt_conf{
         CONFIG::BT::DEVICE_NAME,
         CONFIG::BT::CONTROLLER_MODE,
         CONFIG::BT::MEM_RELEASE_MODE
      };
      bluetooth::init(bt_conf);
      bluetooth::register_auth(Bluetooth::authentication);

      a2dp::connected_cb = A2dp::on_connected;
      a2dp::disconnected_cb = A2dp::on_disconnected;
      a2dp::audio_configure_cb = A2dp::on_audio_configure;
      a2dp::init(A2dp::on_data);

      bluetooth::set_discovery_mode(
         bt_connection_mode::connectable,
         bt_discovery_mode::discoverable
      );

      i2s::config i2s_conf{
         CONFIG::I2S::GENERAL,
         CONFIG::I2S::PINS,
         CONFIG::I2S::PORT,
         CONFIG::I2S::CHANNEL_TYPE,
         CONFIG::I2S::DAC_MODE
      };
      i2s::init(i2s_conf);

      Tasks::show_battery.create();
      Tasks::shutdown.create();
      Tasks::status_blink.create();
      Pins::control_btn.attach_intr(GPIO_INTR_POSEDGE,
         +[](void* arg) {
            Tasks::shutdown.resume_from_isr();
         }
      );
      Pins::show_battery_btn.attach_intr(GPIO_INTR_POSEDGE,
         +[](void* arg) {
            Tasks::show_battery.resume_from_isr();
         }
      );

      Tasks::init_app.destroy();
   }
);

Task Tasks::show_battery(10, 2048, nullptr, "show_battery_task",
   +[](void * arg)
   {
      constexpr uint16_t SHOW_BATTERY_MS = 1500;
      for(;;)
      {
         Tasks::show_battery.suspend();
         LOGM("show battery status");
         Pins::show_battery_display.write(HIGH);
         delay(SHOW_BATTERY_MS);
         Pins::show_battery_display.write(LOW);
      }
   }
);

Task Tasks::shutdown(20, 2048, nullptr, "shutdown_task",
   +[](void* arg)
   {
      constexpr uint16_t SHUTDOWN_AFTER_MS = 3000;
      constexpr uint16_t PERIOD_MS         = 300;
      constexpr uint16_t MAX_COUNTER       = SHUTDOWN_AFTER_MS / PERIOD_MS;
      constexpr uint16_t VIBRATION_START   = MAX_COUNTER - 1;
      constexpr uint16_t VIBRATION_STOP    = 3;

      uint16_t counter = 0;
      for(;;)
      {
         switch(counter)
         {
            case MAX_COUNTER:
               esp_deep_sleep_start();
               break;
            case VIBRATION_START:
               Pins::vibration.write(HIGH);
               break;
            default:
               Pins::vibration.write(LOW);
               break;
         }

         if( Pins::control_btn.read() ) {
            ++counter;
            delay(PERIOD_MS);
         }
         else {
            counter = 0;
            Tasks::shutdown.suspend();
         }
      }
   }
);

Task Tasks::status_blink(10, 2048, nullptr, "status_blink",
   +[](void* arg)
   {
      constexpr uint16_t PERIOD_MS = 600;

      for(;;)
      {
         Pins::status_led.write(HIGH);
         delay(PERIOD_MS);
         Pins::status_led.write(LOW);
         delay(PERIOD_MS);
      }
   }
);


constexpr uint32_t NOT_CONNECTED_TIMER_PERIOD = 10000; // 10s
uint16_t not_connected_counter = 0;
// rings every 30 seconds, if for 3 minutes no one connects, we go in deepsleep
Timer Timers::shutdown_not_connected(ms_to_ticks(NOT_CONNECTED_TIMER_PERIOD),
   true, "startup_shutdown_timer", +[](TimerHandle_t self)
   {
      constexpr uint32_t SHUTDOWN_AFTER_NOT_CONNECTED = 180000; // 3 minutes
      constexpr uint16_t MAX_COUNTER
         = SHUTDOWN_AFTER_NOT_CONNECTED / NOT_CONNECTED_TIMER_PERIOD;

      if( not_connected_counter != MAX_COUNTER ) {
         ++not_connected_counter;
      }
      else {
         LOGM("deepsleep due to no connection");
         esp_deep_sleep_start();
      }
   }
);

void Bluetooth::authentication(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param)
{
   switch (event)
   {
   // case ESP_BT_GAP_AUTH_CMPL_EVT:
   //    if (param->auth_cmpl.stat == ESP_BT_STATUS_SUCCESS) {
   //       LOG( "Auth success" );
   //    }
   //    else {
   //       LOG( "Auth failed" );
   //    }
   //    break;

   case ESP_BT_GAP_CFM_REQ_EVT:
      esp_bt_gap_ssp_confirm_reply(param->cfm_req.bda, true);
      break;

   default:
      break;
   }
}

uint16_t data16[2048];
void A2dp::on_data(const uint8_t* data, uint32_t len)
{
   for( int i = 0; i < 2048; i++ ) {
      data16[i] = ((int16_t*)data)[i] + 0x8000;
   }

   i2s::write( (uint8_t*)data16, len );
}

void A2dp::on_connected(const a2dp::connection_state& param)
{
   Timers::shutdown_not_connected.destroy();
   Tasks::status_blink.destroy();
   Pins::status_led.write(HIGH);
}

void A2dp::on_disconnected(const a2dp::connection_state& param)
{
   not_connected_counter = 0;
   Timers::shutdown_not_connected.create();
   Timers::shutdown_not_connected.start();
   Tasks::status_blink.create();
}

void A2dp::on_audio_configure(const a2dp::audio_config& param)
{
   auto codec_info = param.mcc;
   if( codec_info.type == ESP_A2D_MCT_SBC ) {
      char oct = codec_info.cie.sbc[0];
      i2s::set_clock( a2dp::oct_to_sample_rate(oct) );
   }
}

} // app