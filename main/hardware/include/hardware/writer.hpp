#pragma once
#include <hardware/abstract_hardware_writer.hpp>

namespace hardware {

   class writer : abstract_hardware_writer
   {
   public:
      static writer& instance();
      
      void set_writer( abstract_hardware_writer* ahw );
      
      void configure() override;
      void set_clock( uint16_t sample_rate ) override;
      void write( const uint8_t* data, uint32_t len ) override;

      abstract_hardware_writer* used_writer;
   
   private:
      writer();
      ~writer();
      writer(writer& aw) = delete;
      writer(const writer& aw) = delete;
      
   };
}