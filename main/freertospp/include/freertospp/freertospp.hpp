#pragma once
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <freertos/timers.h>

#include <utility>
#include <functional>

namespace freertospp {

uint32_t ms_to_ticks(uint32_t ms);
uint32_t s_to_ticks(uint8_t s);
void delay(uint32_t ms);

class Task
{
public:
   Task(uint16_t priority, uint16_t stacksize, const char* const name,
         TaskFunction_t func ) : _func(func), _priority(priority),
         _stacksize(stacksize), _name(name), _handle(nullptr)
   {
   }

   void create(void* params = nullptr) const
   {
      xTaskCreate(_func, _name, _stacksize, params, _priority, &_handle);
   }

   void destroy() const
   {
      vTaskDelete(_handle);
   }

   void suspend() const
   {
      vTaskSuspend(_handle);
   }

   void resume() const
   {
      vTaskResume(_handle);
   }

   void resume_from_isr() const
   {
      xTaskResumeFromISR(_handle);
   }

   bool exists() const
   {
      return _handle;
   }

private:
   TaskFunction_t _func;
   uint16_t _priority;
   uint16_t _stacksize;
   const char* const _name;

   mutable TaskHandle_t _handle;
};


class Timer;
typedef void (*TimerFunction)(Timer& timer);

class Timer
{
public:
   Timer(uint32_t period_in_ticks, bool auto_reload, const char* name, TimerCallbackFunction_t func)
      : _period_in_ticks(period_in_ticks), _auto_reload(auto_reload), _name(name),
         _func(func), _id(_timer_counter++)
   {
   }

   bool create(bool start = true) const
   {
      _handle = xTimerCreate(_name, _period_in_ticks, _auto_reload, _id, _func);
      if(start)
         this->start();
      return _handle != nullptr;
   }

   bool start(uint32_t ticks_to_wait = 0) const
   {
      return xTimerStart(_handle, ticks_to_wait) == pdPASS;
   }

   bool stop(uint32_t ticks_to_wait = 0) const
   {
      return xTimerStop(_handle, ticks_to_wait);
   }

   bool destroy(uint32_t ticks_to_wait = 0) const
   {
      return xTimerDelete(_handle, ticks_to_wait) == pdPASS;
   }

   bool period(uint32_t new_period, uint32_t ticks_to_wait = 0)
   {
      _period_in_ticks = new_period;
      return xTimerChangePeriod(_handle, _period_in_ticks, ticks_to_wait) == pdPASS;
   }

   Timer& operator+=(uint16_t ticks_to_add)
   {
      period(_period_in_ticks + ticks_to_add);
      return *this;
   }

private:
   uint32_t _period_in_ticks;
   bool _auto_reload;
   const char* _name;
   TimerCallbackFunction_t _func; // TODO don't like this
   uint32_t _id;

   mutable TimerHandle_t _handle;
   static uint16_t _timer_counter;
};


template< typename T, uint16_t LEN>
class Queue
{
public:
   Queue()
   {
      _handle = xQueueCreate(100, sizeof(T));
   }

   std::pair<bool, T> pop()
   {
      T item;
      bool success = xQueueReceive(_handle, &item, portMAX_DELAY);
      return std::make_pair(success, item);
   }

   bool pop_into(T& item)
   {
      return xQueueReceive(_handle, &item, portMAX_DELAY);
   }

   bool push(const T&);
   void push_from_isr(const T& item)
   {
      xQueueSendFromISR(_handle, &item, nullptr);
   }

private:


   mutable QueueHandle_t _handle;
};

}