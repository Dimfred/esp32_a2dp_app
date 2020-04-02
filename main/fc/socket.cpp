#include <fc/socket.hpp>
#include <fc/utilities.hpp>

#include <cstring>

namespace fc {
   
   socket& socket::instance()
   {
      static socket s;
      return s;
   }

   void socket::close()
   {
      shutdown( _sock, 0 );
      ::close( _sock );
      _sock = -1;
   }

   bool socket::has_descriptor() const
   {
      return _sock > -1;
   }

   void socket::create_client()
   { try {

      if( _MODE == CLIENT )
         return;

      if( has_descriptor() ) {
         close();
      }

      _send_addr.sin_family         = CONFIG::NET::ADDR_FAMILY,
      _send_addr.sin_len            = sizeof( _send_addr );
      _send_addr.sin_addr.s_addr    = htonl( INADDR_BROADCAST ); //inet_addr( CONFIG::NET::CLIENT_SEND_ADDR ); //lwip_htonl( CONFIG::NET::CLIENT_SEND_ADDR );
      _send_addr.sin_port           = htons( CONFIG::NET::PORT );
      
      sockaddr_in bind_addr;
      bind_addr.sin_family = CONFIG::NET::ADDR_FAMILY;
      bind_addr.sin_len = sizeof( bind_addr );
      bind_addr.sin_addr.s_addr = htonl( INADDR_ANY );
      bind_addr.sin_port = htonl( 6001 ); 

      _sock = lwip_socket( CONFIG::NET::ADDR_FAMILY, CONFIG::NET::SOCK_TYPE,
         CONFIG::NET::IP_PROTOCOL );
      if( _sock < 0 ) // TODO NEW EXCEPTION FOR SOCKS
         FC_EXCEPTION( _sock, "socket for client could not be created" );

      int32_t err = lwip_setsockopt( _sock, CONFIG::NET::SOCK_LEVEL, CONFIG::NET::CLIENT_SOCK_OPTION,
         &CONFIG::NET::SOCK_OPTIONAL, sizeof(CONFIG::NET::SOCK_OPTIONAL) );
      FC_EXCEPTION( err, "socket failed to set client socketopt" );

      err = lwip_bind( _sock, (struct sockaddr*) &bind_addr, sizeof(bind_addr) );
      FC_EXCEPTION( err, "socket bind failed" );


      _MODE = CLIENT;
      LOGM( "client created" );

   } FC_LOG_AND_RETHROW() }
   
   void socket::create_server()
   { try {

      if( _MODE == SERVER )
         return;

      if( has_descriptor() ) {
         close();
      }

      _receive_addr.sin_family         = CONFIG::NET::ADDR_FAMILY,
      _receive_addr.sin_len            = sizeof(_receive_addr);      
      _receive_addr.sin_addr.s_addr    = inet_addr( CONFIG::NET::SERVER_BIND_ADDR );
      _receive_addr.sin_port           = htons( CONFIG::NET::PORT );

      _sock = ::socket( CONFIG::NET::ADDR_FAMILY, CONFIG::NET::SOCK_TYPE,
         CONFIG::NET::IP_PROTOCOL );
      if( _sock < 0 ) // TODO better exception
         FC_EXCEPTION( _sock, "socket could not be created" );

      int32_t err = setsockopt( _sock, CONFIG::NET::SOCK_LEVEL, CONFIG::NET::SERVER_SOCK_OPTION,
         &CONFIG::NET::SOCK_OPTIONAL, sizeof(CONFIG::NET::SOCK_OPTIONAL) );
      FC_EXCEPTION( err, "socket failed set server sockopt" );
      
      err = bind( _sock, (struct sockaddr*) &_receive_addr, sizeof(_receive_addr) );
      FC_EXCEPTION( err, "socket failed to bind" );

      _MODE = SERVER;
      LOGM( "server created" );
      
   } FC_LOG_AND_RETHROW() }

   int32_t socket::receive( uint8_t* data, size_t len )
   {
      // TODO REMOVE LATER
      static uint16_t read_counter;
      if( ++read_counter % 200 == 0 )
         LOGE();

      int32_t read_bytes = recv( _sock, data, len, 0 );
      return read_bytes;
   }

   int32_t socket::send( const uint8_t* data, size_t len )
   {
      // TODO REMOVE LATER
      static uint16_t write_counter;
      if( ++write_counter % 200 == 0 )
         LOGE();

      //udp_sendto( );
      int32_t send_bytes = lwip_sendto( _sock, data, len, 0,
         (const struct sockaddr*) &_send_addr, sizeof(_send_addr) );
      return send_bytes;
   }

} // fc
