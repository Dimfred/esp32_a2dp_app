#pragma once
#include <esp_bt.h>
#include <driver/i2s.h>
#include <esp_wifi_types.h>

namespace CONFIG {

   #define USE_INTERNAL_DAC 1
   struct PINS
   {
      static constexpr uint16_t ON_OFF_BTN           = 34;
      static constexpr uint16_t SHOW_BATTERY_BTN     = 27;
      static constexpr uint16_t SHOW_BATTERY_DISPLAY = 18;
      static constexpr uint16_t VIBRATION            = 32;
      static constexpr uint16_t STATUS_LED           = 23;

      static constexpr uint16_t I2S_BCK  = 26;
      static constexpr uint16_t I2S_WS   = 22;
      static constexpr uint16_t I2S_DOUT = 25;
      static constexpr uint16_t I2S_DIN  = -1;

      PINS() = delete;
   };

   struct BT
   {
      static constexpr char* DEVICE_NAME = "MY_BLUETOOTH_DEVICE_NAME";
      static constexpr esp_bt_mode_t CONTROLLER_MODE = ESP_BT_MODE_CLASSIC_BT;
      static constexpr esp_bt_mode_t MEM_RELEASE_MODE = ESP_BT_MODE_BLE;

      BT() = delete;
   };

   struct I2S
   {
      static constexpr i2s_dac_mode_t DAC_MODE =
         #if USE_INTERNAL_DAC
            I2S_DAC_CHANNEL_BOTH_EN;
         #else
            I2S_DAC_CHANNEL_DISABLE;
         #endif

      static constexpr i2s_config_t GENERAL =
      {
         #if USE_INTERNAL_DAC
            mode               : I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN,
         #else
            mode               : I2S_MODE_MASTER | I2S_MODE_TX,
         #endif
            sample_rate        : 44100,
            bits_per_sample    : I2S_BITS_PER_SAMPLE_16BIT,
            channel_format     : I2S_CHANNEL_FMT_RIGHT_LEFT,
            communication_format : I2S_COMM_FORMAT_PCM, //I2S_COMM_FORMAT_PCM, //I2S_COMM_FORMAT_I2S_MSB,
            intr_alloc_flags   : 0,
            dma_buf_count      : 6,
            dma_buf_len        : 60,
            use_apll           : false,
            tx_desc_auto_clear : true,
            fixed_mclk         : false
      };

      static constexpr i2s_pin_config_t PINS
      {
         bck_io_num   : PINS::I2S_BCK,
         ws_io_num    : PINS::I2S_WS,
         data_out_num : PINS::I2S_DOUT,
         data_in_num  : PINS::I2S_DIN
      };

      static constexpr i2s_port_t       PORT = I2S_NUM_0;
      static constexpr i2s_channel_t    CHANNEL_TYPE = I2S_CHANNEL_STEREO;

      I2S() = delete;
   };

} // CONFIG
