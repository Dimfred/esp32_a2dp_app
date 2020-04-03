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

App::state App::next_state = App::wakeup;
App::state App::prev_state = App::wakeup;
Task App::task(25, 2048, "app_task", +[](void* arg)
{
   for(;;)
   {
      switch( next_state )
      {
         case wakeup:
         {
            LOGM("state wakeup");
            Wakeup::task.create();
            break;
         }
         case startapp:
         {
            LOGM("state startapp")
            Common::vibration_task.create(Common::VIBRATION_TIME);
            Common::show_battery_task.create(Common::SHOW_BATTERY_FOR_MS);
            Common::shutdown_task.create(Common::SHUTDOWN_AFTER_MS);
            Startapp::init_app.create();
            break;
         }
         case disconnected:
         {
            LOGM("state disconnected");
            Disconnected::shutdown_timer.create();
            Disconnected::status_blink_task.create(Disconnected::BLINK_PERIOD_MS);
            break;
         }
         case connected:
         {
            LOGM("state connected");
            Disconnected::shutdown_timer.destroy();
            Disconnected::status_blink_task.destroy();
            Pins::status_led.write(HIGH);
            break;
         }
         case shutdown:
         {
            LOGM("state shutdown");
            if( prev_state != wakeup ) {
               Common::vibration_task.create(Common::VIBRATION_TIME);
               delay(Common::VIBRATION_TIME);
            }

            esp_deep_sleep_start();
            break;
         }
      }

      App::task.suspend();
   }
});

void App::run()
{
   App::task.create();
}

void App::apply_next_state(state state)
{
   App::prev_state = App::next_state;
   App::next_state = state;
   App::task.resume();
}

Task Wakeup::task(25, 2048, "wakeup_task", +[](void* arg)
{
   gpio_install_isr_service(0);
   esp_sleep_enable_ext0_wakeup(Pins::control_btn, HIGH);

   Timer shutdown_timer(ms_to_ticks(1000), false, "shutdown_timer", +[](TimerHandle_t self) {
      App::apply_next_state(App::shutdown);
   });
   shutdown_timer.create();

   constexpr uint16_t MAX_BUTTON_PRESS = HOLD_BUTTON_FOR_MS / TASK_PERIOD_MS;
   for( uint16_t button_press = 0; button_press != MAX_BUTTON_PRESS; )
   {
      if( Pins::control_btn.read() ) {
         ++button_press;
         shutdown_timer += TASK_PERIOD_MS;
      }

      delay(TASK_PERIOD_MS);
   }
   shutdown_timer.destroy();

   App::apply_next_state(App::startapp);

   task.destroy();
});

Task Startapp::init_app(21, 4096, "init_app_task", +[](void* arg)
{
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

   Pins::control_btn.attach_intr(GPIO_INTR_POSEDGE,
      +[](void* arg) {
         Common::shutdown_task.resume_from_isr();
      }
   );
   Pins::show_battery_btn.attach_intr(GPIO_INTR_POSEDGE,
      +[](void* arg) {
         Common::show_battery_task.resume_from_isr();
      }
   );

   App::apply_next_state(App::disconnected);

   init_app.destroy();
});

Task Disconnected::status_blink_task(10, 2048, "status_blink_task", +[](void* arg)
{
   uint16_t PERIOD_MS = arg;

   for(;;)
   {
      Pins::status_led.write(HIGH);
      delay(PERIOD_MS);
      Pins::status_led.write(LOW);
      delay(PERIOD_MS);
   }
});

uint32_t Disconnected::shutdown_time_passed_ms = 0;
Timer Disconnected::shutdown_timer(ms_to_ticks(SHUTDOWN_TIMER_PERIOD_MS), true, "startup_shutdown_timer",
   +[](TimerHandle_t self)
   {
      constexpr uint32_t SHUTDOWN_AFTER_MS = SHUTDOWN_AFTER_S * 1000;

      if( shutdown_time_passed_ms != SHUTDOWN_AFTER_MS )
         shutdown_time_passed_ms += SHUTDOWN_TIMER_PERIOD_MS;
      else
         App::apply_next_state(App::shutdown);
   }
);

Task Common::shutdown_task(20, 2048, "shutdown_task", +[](void* arg)
{
   uint16_t SHUTDOWN_AFTER_MS = arg;
   const uint16_t MAX_BUTTON_PRESS = SHUTDOWN_AFTER_MS / SHUTDOWN_TASK_PERIOD_MS;

   uint16_t button_press = 0;
   while( button_press < MAX_BUTTON_PRESS )
   {
      if( Pins::control_btn.read() )
      {
         ++button_press;
         delay(SHUTDOWN_TASK_PERIOD_MS);
      }
      else
      {
         button_press = 0;
         shutdown_task.suspend();
      }
   }

   App::apply_next_state(App::shutdown);

   shutdown_task.destroy();
});

Task Common::show_battery_task(10, 2048, "show_battery_task", +[](void * arg)
{
   uint16_t show_battery_for_ms = arg;
   for(;;)
   {
      show_battery_task.suspend();

      LOGM("show battery status");
      Pins::show_battery_display.write(HIGH);
      delay(show_battery_for_ms);
      Pins::show_battery_display.write(LOW);
   }
});

Task Common::vibration_task(15, 2048, "vibration_task", +[](void* arg)
{
   uint16_t VIBRATION_TIME = arg;
   Pins::vibration.write(HIGH);
   delay(VIBRATION_TIME);
   Pins::vibration.write(LOW);

   vibration_task.destroy();
});

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
   App::apply_next_state(App::connected);
}

void A2dp::on_disconnected(const a2dp::connection_state& param)
{
   App::apply_next_state(App::disconnected);
}

void A2dp::on_audio_configure(const a2dp::audio_config& param)
{
   auto codec_info = param.mcc;
   if( codec_info.type == ESP_A2D_MCT_SBC ) {
      char oct = codec_info.cie.sbc[0];
      i2s::set_clock( a2dp::oct_to_sample_rate(oct) );
   }
}

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

} // app