/*
 *	__  __ ______ _______ _______ _______ ______ 
 *	|  |/  |   __ \   |   |     __|    ___|   __ \
 *	|     <|      <   |   |    |  |    ___|      <
 *	|__|\__|___|__|_______|_______|_______|___|__|
 *	       H E A V Y  I N D U S T R I E S
 *
 *	Copyright (C) 2014 Krüger Heavy Industries
 *	http://www.krugerheavyindustries.com
 *
 * Written by Chris Kruger AT krugerheavyindustries.com
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above
 *   copyright notice, this list of conditions and the following
 *   disclaimer in the documentation and/or other materials provided
 *   with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "users.h"
#include "confuse.h"
#include "exception.h"

#define _PATH_TAIKODCONF "/etc/taiko.conf"

//#define _PATH_TAIKODCONF "taiko.conf"

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
