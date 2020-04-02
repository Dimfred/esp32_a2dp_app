#pragma once
#include <driver/gpio.h>
#include <fc/utilities.hpp>

namespace hw {

class pin_config_builder;

#define HIGH 1
#define LOW 0

#define INPUT GPIO_MODE_INPUT
#define OUTPUT GPIO_MODE_OUTPUT

class pin
{
public:
   pin(uint16_t num, gpio_mode_t mode): _self(num), _mode(mode)
   {
      _config(num, mode, GPIO_INTR_DISABLE);
   }

   void attach_intr(gpio_int_type_t intr_type, gpio_isr_t handler)
   {
      _config((uint16_t)_self, _mode, intr_type);
      gpio_isr_handler_add(_self, handler, (void*)_self);
   }

   uint16_t read() const
   {
      return gpio_get_level(_self);
   }

   void write(bool lvl) const
   {
      gpio_set_level(_self, lvl);
   }

   operator uint16_t() const
   {
      return _self;
   }

private:
   gpio_num_t _self;
   gpio_mode_t _mode;

   void _config(uint16_t num, gpio_mode_t mode, gpio_int_type_t intr_type)
   {
      gpio_config_t io_conf;
      io_conf.intr_type = intr_type;
      io_conf.mode = mode;
      io_conf.pin_bit_mask = (1ULL << num);
      gpio_config(&io_conf);
   }
};

} // hw

