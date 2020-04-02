#pragma once

#include <esp_err.h>

#include <string>
#include <exception>

namespace fc
{
class exception;

#define FC_EXCEPTION( ERROR_CODE, MESSAGE ) \
   { \
      static_cast<std::string>(MESSAGE); \
      if( ERROR_CODE ) \
         throw fc::exception( ERROR_CODE, MESSAGE ); \
   }
   

class exception : public std::exception
{   
   public:
      exception( esp_err_t err, const char* message ) 
         : error_code(err), _message(message) {}

      const char* what() const throw()
      {
         return _message;
      } 

      const char* error_code_message() const 
      {
         return esp_err_to_name(error_code);
      }

      esp_err_t   error_code;
   private:  
      const char* _message;
};

} // fc