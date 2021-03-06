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
      
      void parse_user_config();
   };
   
} // namespace taiko

#endif /* defined(_TAIKO_USERS_T_) */
