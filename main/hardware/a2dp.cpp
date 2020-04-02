#include <hardware/a2dp.hpp>
#include <fc/exception.hpp>
#include <fc/utilities.hpp>

namespace hw {

a2dp::connection_state_cb a2dp::connected_cb = nullptr;
a2dp::connection_state_cb a2dp::disconnected_cb = nullptr;

a2dp::audio_state_cb a2dp::audio_started_cb = nullptr;
a2dp::audio_state_cb a2dp::audio_stopped_cb = nullptr;
a2dp::audio_state_cb a2dp::audio_suspended_cb = nullptr;

a2dp::audio_config_cb a2dp::audio_configure_cb = nullptr;


static void event_loop(esp_a2d_cb_event_t event, esp_a2d_cb_param_t* params);

void a2dp::init(esp_a2d_sink_data_cb_t data_cb)
{
   esp_err_t err = esp_a2d_register_callback(event_loop);
   FC_EXCEPTION( err, "a2dp event callback could not be registerd" );
   LOGM( "a2dp event callback registered" );

   err = esp_a2d_sink_register_data_callback( data_cb );
   FC_EXCEPTION( err, "a2dp data callback could not be registered" );
   LOGM( "a2dp data callback registered" );

   err = esp_a2d_sink_init();
   FC_EXCEPTION( err, "a2dp sink could not be initialized" );
   LOGM( "a2dp sink initialized" );
}

int32_t a2dp::oct_to_sample_rate(uint8_t oct)
{
   int32_t sample_rate = 16000;
   if (oct & (0x01 << 6)) {
      sample_rate = 32000;
   }
   else if (oct & (0x01 << 5)) {
      sample_rate = 44100;
   }
   else if (oct & (0x01 << 4)) {
      sample_rate = 48000;
   }

   return sample_rate;
}

void event_loop(esp_a2d_cb_event_t event, esp_a2d_cb_param_t* params)
{
   switch( event )
   {
      case ESP_A2D_CONNECTION_STATE_EVT:
      {
         auto connection_state = params->conn_stat;
         switch( connection_state.state )
         {
            case ESP_A2D_CONNECTION_STATE_CONNECTED:
               if( a2dp::connected_cb != nullptr ) {
                  LOGM("a2dp connected");
                  a2dp::connected_cb(connection_state);
               }
               break;
            case ESP_A2D_CONNECTION_STATE_DISCONNECTED:
               if( a2dp::disconnected_cb != nullptr ) {
                  LOGM("a2dp disconnected");
                  a2dp::disconnected_cb(connection_state);
               }
               break;
            default:
               break;
         }
         break;
      }
      case ESP_A2D_AUDIO_STATE_EVT:
      {
         auto audio_state = params->audio_stat;
         switch( audio_state.state )
         {
            case ESP_A2D_AUDIO_STATE_STARTED:
               if( a2dp::audio_started_cb != nullptr ) {
                  LOGM( "a2dp audio transmission started" );
                  a2dp::audio_started_cb(audio_state);
               }
               break;
            case ESP_A2D_AUDIO_STATE_STOPPED:
               if( a2dp::audio_stopped_cb != nullptr ) {
                  LOGM( "a2dp audio transmission stopped" );
                  a2dp::audio_stopped_cb(audio_state);
               }
               break;
            case ESP_A2D_AUDIO_STATE_REMOTE_SUSPEND:
               if( a2dp::audio_stopped_cb != nullptr ) {
                  LOGM( "a2dp audio remote suspended" );
                  a2dp::audio_suspended_cb(audio_state);
               }
               break;
            default:
               break;
         }
         break;
      }
      case ESP_A2D_AUDIO_CFG_EVT:
      {
         auto audio_config = params->audio_cfg;
         if( a2dp::audio_configure_cb != nullptr ) {
            LOGM("a2dp audio configure");
            a2dp::audio_configure_cb(audio_config);
         }
         break;
      }
      default:
         LOGMV("a2dp other event", event);
         break;
   }
}

} // hw