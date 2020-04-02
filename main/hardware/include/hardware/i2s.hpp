#pragma once
#include <driver/i2s.h>

namespace hw { namespace i2s {

struct config
{
   i2s_config_t     general;
   i2s_pin_config_t pins;
   i2s_port_t       port;
   i2s_channel_t    channel;
   i2s_dac_mode_t   dac_mode;
};

void init(const config& config);
void set_clock( int32_t sample_rate );
void write(const uint8_t* data, uint32_t len );

}} // hw::i2s