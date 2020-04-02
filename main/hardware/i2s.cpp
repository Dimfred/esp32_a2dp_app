#include <hardware/i2s.hpp>
#include <fc/config.hpp>
#include <fc/utilities.hpp>
#include <fc/exception.hpp>


namespace hw { namespace i2s {

static config _config;

void init(const config& config)
{
   _config = config;

   LOG(_config.general.communication_format);

   esp_err_t err = i2s_driver_install(
      _config.port,
      &_config.general,
      0,
      nullptr
   );
   FC_EXCEPTION( err, "i2s failed to install driver" );
   LOGM( "i2s driver installed" );

   if( _config.dac_mode )
   {
      err = i2s_set_dac_mode(_config.dac_mode);
      FC_EXCEPTION( err, "i2s failed to set dac mode" );
      LOGM("i2s set dac mode");

      err = i2s_set_pin( _config.port, nullptr );
      FC_EXCEPTION( err, "i2s failed to set pin" );
      LOGM("i2s set pin for dac mode");
   }
   else
   {
      err = i2s_set_pin( _config.port, &_config.pins );
      FC_EXCEPTION( err, "i2s failed to set pin" );
      LOGM("i2s set pin for non dac mode");
   }
}

void set_clock( int32_t sample_rate )
{
   esp_err_t err = i2s_set_clk(
      _config.port,
      sample_rate,
      _config.general.bits_per_sample,
      _config.channel
   );

   FC_EXCEPTION( err, "i2s failed to set clock" );
   LOGMV( "i2s clock set", sample_rate );
}

void write( const uint8_t* data, uint32_t len )
{
   size_t bytes_written;
   esp_err_t err = i2s_write(
      _config.port,
      data,
      len,
      &bytes_written,
      portMAX_DELAY
   );
   FC_EXCEPTION( err, "i2s error while writing" );
}

}} // hw::i2s