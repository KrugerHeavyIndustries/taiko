#ifndef _TAIKO_EXCEPTION_H_
#define _TAIKO_EXCEPTION_H_

#include <exception>

namespace taiko {
   
class exception : public std::exception {
public:
   exception(const string& m) :
      _what(m) {}

   virtual ~exception() throw() {
   }
   
   virtual const std::string what() {
      return _what;
   }
   
private:
   
   std::string _what;
};

class exception_notfound : public exception {
public:
   explicit exception_notfound(const string& m)
   :  exception(m) {
   }
   virtual ~exception_notfound() throw() {
   }

};
   
}
#endif // _TAIKO_EXCEPTION_H_
