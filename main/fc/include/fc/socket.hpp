#pragma once
#include <fc/config.hpp>
#include <fc/utilities.hpp>

#include <lwip/netdb.h>
#include <lwip/sockets.h>
#include <lwip/ip_addr.h>

namespace fc {

class socket 
{
public:
   static socket& instance();
   
   void create_client();
   void create_server();
   int32_t receive( uint8_t* data, size_t len );
   int32_t send( const uint8_t* data, size_t len );
   void close();
   bool has_descriptor() const; 
   
private:
   socket(){}
   ~socket(){}
   socket( socket& s ) = delete;
   socket( const socket& s ) = delete;

   enum MODE
   {
      SERVER, 
      CLIENT,
      NONE
   };

   int32_t _sock = -1;
   sockaddr_in _send_addr;
   sockaddr_in _receive_addr;
   MODE _MODE = NONE;
   
};

} // fc



