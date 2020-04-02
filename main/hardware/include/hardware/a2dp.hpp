#pragma once
#include <esp_a2dp_api.h>

namespace hw {

class a2dp
{
public:
   typedef esp_a2d_cb_param_t::a2d_conn_stat_param connection_state;
   typedef esp_a2d_cb_param_t::a2d_audio_stat_param audio_state;
   typedef esp_a2d_cb_param_t::a2d_audio_cfg_param audio_config;

   typedef void (*connection_state_cb)(const connection_state&);
   typedef void (*audio_state_cb)(const audio_state&);
   typedef void (*audio_config_cb)(const audio_config&);

   static void init(esp_a2d_sink_data_cb_t data_cb);
   static int32_t oct_to_sample_rate(uint8_t oct);

   static connection_state_cb connected_cb;
   static connection_state_cb disconnected_cb;

   static audio_state_cb audio_started_cb;
   static audio_state_cb audio_stopped_cb;
   static audio_state_cb audio_suspended_cb;

   static audio_config_cb audio_configure_cb;
private:

   a2dp() = delete;
};

}