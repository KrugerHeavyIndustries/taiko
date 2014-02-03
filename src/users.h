//
//  users_t.h
//  openidd
//
//  Created by Chris Kruger on 30/01/2014.
//
//

#ifndef _TAIKO_USERS_T_
#define _TAIKO_USERS_T_

#include <string>
#include <map>
#include <iostream>

namespace taiko {
   
   using namespace std;
   
   class user_t {
      
   private:
      
      string name;
      
      string email;
      
   public:
      
      user_t(const string& n,const string& e)
            : name(n), email(e) { }
      
      void set_name(const string& n);
                    
      void set_email(const string& e);
                    
      const string& get_name() const { return name; }
      
      const string& get_email() const { return email; }
 
   };
   
   class users_t : public multimap<string, user_t>
   {
   public:
      
      users_t() { }
      
      bool has_user(const string& un) const;
      
      void set_user(const string& un, const user_t& u);
      
      const user_t& get_user(const string& un) const;
      
      const user_t get_user(const string& un);
      
      void parse_user_config();
   };
   
} // namespace taiko

#endif /* defined(_TAIKO_USERS_T_) */
