#include <vector>
#include <functional>

namespace fc {

template< class T >
class signal;

template< class Ret, class... Args >
class signal<Ret(Args...)>
{
public:
   typedef std::function<Ret(Args...)> callback;
   typedef typename std::vector<callback> callbacks;
   typedef uint16_t connection;

   connection connect( callback cb )
   {
      _callbacks.push_back(cb);
      return _callbacks.size()-1;
   }
   void disconnect( connection c )
   {
      auto del = _callbacks.begin();
      std::advance( del, c );
      _callbacks.erase(del);
   }

   inline void emit(Args... arg)
   {
      for( callback cb : _callbacks )
         cb(arg...);
   }

   inline void operator()(Args... arg)
   {
     emit(arg...);
   }

private:
   callbacks _callbacks;
};

} // fc