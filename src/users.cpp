//  openidd
//
//  Created by Chris Kruger on 30/01/2014.
//
//

#include "users.h"
#include "confuse.h"
#include "exception.h"

#define _PATH_TAIKODCONF "/etc/taikod.conf" 

namespace taiko {
   
   int validate_user_callback(cfg_t* cfg, cfg_opt_t* opt) {
      return 0;
   }

   void user_t::set_name(const string& n) {
      name = n;
   }
   
   void user_t::set_email(const string& e) {
      email = e;
   }
   
   bool users_t::has_user(const string& un) const {
      return find(un) != end();
   }
   
   void users_t::set_user(const string& un, const user_t& u) {
      insert(value_type(un, u));
   }
   
   const user_t& users_t::get_user(const string& un) const {
      const_iterator i = find(un);
      if (i == end())
         throw  taiko::exception("failed to find user");
      return i->second;
   }
      
   void users_t::parse_user_config() {
      cfg_t* cfg;
      
      cfg_opt_t user_opts[] = {
         CFG_STR("name", 0, CFGF_NONE),
         CFG_STR("email", 0, CFGF_NONE),
         CFG_END()
      };
      
      cfg_opt_t opts[] = {
         CFG_SEC("user", user_opts, CFGF_MULTI | CFGF_TITLE),
         CFG_END()
      };
      
      cfg = cfg_init(opts, CFGF_NOCASE);
      
      cfg_set_validate_func(cfg, "user", &validate_user_callback);
      
      switch (cfg_parse(cfg, _PATH_TAIKODCONF))
      {
         case CFG_FILE_ERROR:
            break;
         case CFG_PARSE_ERROR:
            break;
      };
      
      int user_count = cfg_size(cfg, "user");
      for (int i = 0; i < user_count; ++i)
      {
         cfg_t* user_attributes = cfg_getnsec(cfg, "user", i);
         set_user(cfg_title(user_attributes),
                  user_t(cfg_getstr(user_attributes, "name"), cfg_getstr(user_attributes, "email")));
      }
      
      cfg_free(cfg);
   }

}
