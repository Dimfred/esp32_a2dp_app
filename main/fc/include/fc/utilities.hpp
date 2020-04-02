#pragma once
#include <fc/exception.hpp>

#include <iostream>
#include <string>
#include <algorithm>
#include <cstdint>

namespace fc {

template<typename T>
void log( std::string varname, T var, std::string FILE, std::string FUNC, uint16_t LINE )
{
   FILE = std::string( ( std::find(FILE.rbegin(),
      FILE.rend(), '/') ).base(), FILE.end() );

   std::cout << "\033[1;37m" << FILE << ":" << LINE << "\t" << FUNC << "\033[0m\n";
   if( !varname.empty() )
      std::cout << varname << ": ";
   std::cout << var <<std::endl;
}

#define LOG_ENABLED

//#undef LOG_ENABLED


#ifdef LOG_ENABLED

   #define LOG( var )                                    \
      fc::log( #var, var, __FILE__, __func__, __LINE__ );

   #define LOGM( message )                               \
      fc::log( "", message, __FILE__, __func__, __LINE__ );

   #define LOGMV( message, variable )                    \
      fc::log( message, variable, __FILE__, __func__, __LINE__ );

   #define LOGE() \
      fc::log( "", "", __FILE__, __func__, __LINE__ );

#else

   #define LOG( var )
   #define LOGM( message )
   #define LOGMV( message, variable )

#endif

#define FC_LOG_AND_RETHROW()                             \
   catch(fc::exception &e)                               \
   {                                                     \
      std::string FILE = __FILE__;                       \
      FILE = std::string( ( std::find(FILE.rbegin(),     \
         FILE.rend(), '/') ).base(), FILE.end() );       \
      std::cout << "\033[1;31mException \033[0;mat: " << FILE << ":" << __LINE__ << "\n"  \
         << "What: " << e.what() << "\n"                             \
         << "Code: " << e.error_code << ": " << e.error_code_message() << std::endl;         \
      throw e;                                           \
   }

#define FC_LOG_AND_CATCH()                               \
   catch( fc::exception& e)                              \
   {                                                     \
      std::string FILE = __FILE__;                       \
      FILE = std::string( ( std::find(FILE.rbegin(),     \
            FILE.rend(), '/') ).base(), FILE.end() );    \
         std::cout << "\033[1;31mException at: " << FILE << ":" << __LINE__ << "\033[0;m\n"      \
            << "What: " << e.what() << "\n"                          \
            << "Code: " << e.error_code << ": " << e.error_code_message() << std::endl;      \
   }

} // fc