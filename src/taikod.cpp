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
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <opkele/verify_op.h>
#include <opkele/exception.h>
#include <opkele/association.h>
#include <opkele/util.h>
#include <opkele/uris.h>
#include <opkele/sreg.h>
#include <uuid/uuid.h>
#include <sys/stat.h>
#include <unistd.h>
#include <syslog.h>
#include "sqlite.h"
#include "utils.h"
#include "cookies.h"
#include "mongoose.h"
#include "kingate_openid_message.h"

#include "auth_bsd.h" 
#include "users.h"

#include "taiko_op.h"

#define ENFORCE_SSL 1

using namespace std;
using namespace taiko;

users_t users;

static int handle_request(struct mg_connection* conn) {
   
   mongoose_connection_t mc(conn);
   
   ostringstream status;
   ostringstream header;
   ostringstream content_type;
   ostringstream content;

#if defined(ENFORCE_SSL)
   if (!mc.is_ssl())
      throw taiko::exception("SSL connection required");
#endif 
   
   string op;
   if (mc.has_param("op"))
      op = mc.get_param("op");
   
   string message;
   if (op=="login") {
      taiko_op_t OP(mc);
      string username = mc.get_param("username");
      string password = mc.get_param("password");
      if (users.has_user(username)) {
         if (check_auth(username.c_str(), password.c_str()) == STATUS_OK) {
            OP.set_authorized(true);
            OP.set_username(username);
            op.clear();
            if (mc.has_param("memoized_params")) {
               string memoized_params = mc.get_param("memoized_params");
               status << "HTTP/1.1 302 Going back to OP with checkid_setup after successful login\r\n";
               header << "Location: " << get_self_url(mc) << "?" << memoized_params << "\r\n";
            } else {
               status << "HTTP/1.1 200 OK\r\n";
            }
            OP.cookie_header(header);
         } else {
            throw taiko::exception("wrong password or username");
         }
      } else {
         throw taiko::exception("wrong password or username");
      }
   } else if(op=="logout") {
      taiko_op_t OP(mc);
      OP.set_authorized(false);
      op.clear();
      message = "logged out";
   }
   string omode;
   if (mc.has_param("openid.mode"))
      omode = mc.get_param("openid.mode");
   if (op=="xrds") {
      content_type <<
      "Content-Type: application/xrds+xml\r\n";
      content <<
      "<?xml version='1.0' encoding='utf-8'?>"
      "<xrds:XRDS xmlns:xrds='xri://$xrds' xmlns='xri://$xrd*($v*2.0)'>"
      "<XRD>"
      "<Service>"
      "<Type>" STURI_OPENID20 "</Type>"
      "<URI>" << get_self_url(mc) << "</URI>"
      "</Service>";
      if(mc.has_param("idsel")){
         content <<
         "<Service>"
         "<Type>" STURI_OPENID20_OP "</Type>"
         "<URI>" << get_self_url(mc) << "</URI>";
      }
      content <<
      "</XRD>"
      "</xrds:XRDS>";
   } else if (op=="id_res" || op=="cancel") {
      kingate_openid_message_t inm(mc);
      taiko_op_t OP(mc);
      if (mc.get_param("hts_id") != OP.htc.get_value())
         throw taiko::exception("toying around, huh?");
      opkele::sreg_t sreg;
      OP.checkid_(inm, sreg);
      OP.cookie_header(header);
      opkele::openid_message_t om;
      if (op=="id_res") {
         if(!OP.get_authorized())
            throw taiko::exception("not logged in");
         if(OP.is_id_select()) {
            OP.select_identity(get_self_url(mc), get_self_url(mc));
         }
         string username = OP.get_username();
         if (!users.has_user(username)) {
            throw taiko::exception("user not setup to use taiko");
         }
         const user_t& user = users.get_user(username);
         sreg.set_field(opkele::sreg_t::field_fullname,user.get_name());
         sreg.set_field(opkele::sreg_t::field_email,user.get_email());
         sreg.setup_response();
         status << "HTTP/1.1 302 Going back to RP with id_res\r\n";
         header <<
         "Location: " << OP.id_res(om, sreg).append_query(OP.get_return_to())
         << "\r\n";
      } else {
         status << "HTTP/1.1 302 Going back to RP with cancel\r\n";
         header <<
         "Location: " << OP.cancel(om).append_query(OP.get_return_to())
         << "\r\n";
      }
      om.to_keyvalues(clog);
   }else if(omode=="associate") {
      const kingate_openid_message_t inm(mc);
      opkele::openid_message_t oum;
      taiko_op_t OP(mc);
      OP.associate(oum,inm);
      content_type << "Content-Type: text/plain\r\n";
      oum.to_keyvalues(content);
   }else if(omode=="checkid_setup") {
      kingate_openid_message_t inm(mc);
      taiko_op_t OP(mc);
      OP.checkid_(inm,0);
      content_type <<
      "Content-Type: text/html\r\n";
      OP.cookie_header(header);
      
      if (OP.get_authorized()) {
         content <<
         "<html>"
         "<head>"
         "<title>test OP: confirm authentication</title>"
         "</head>"
         "<body>"
         "realm: " << OP.get_realm() << "<br/>"
         "return_to: " << OP.get_return_to() << "<br/>"
         "claimed_id: " << OP.get_claimed_id() << "<br/>"
         "identity: " << OP.get_identity() << "<br/>";
         if(OP.is_id_select()) {
            OP.select_identity(get_self_url(mc), get_self_url(mc));
            content <<
            "selected claimed_id: " << OP.get_claimed_id() << "<br/>"
            "selected identity: " << OP.get_identity() << "<br/>";
         }
         content <<
         "<form method='post'>";
         inm.to_htmlhiddens(content, "openid.");
         content <<
         "<input type='hidden' name='hts_id'"
         " value='" << opkele::util::attr_escape(OP.htc.get_value()) << "'/>"
         "<input type='submit' name='op' value='id_res'/>"
         "<input type='submit' name='op' value='cancel'/>"
         "</form>"
         "</body>"
         "</html>";
         
      } else {
         content <<
         "<html>"
         "<head>"
         "<title>test OP: authentication</title>"
         "</head>"
         "<body>"
         "<form method='post'>"
         "login "
         "<input type='hidden' name='op' value='login'/>"
         "<input type='hidden' name='memoized_params' value='" << inm.query_string() << "'/>" <<
         "<input type='username' name='username' value=''/>"
         "<input type='password' name='password' value=''/>"
         "<input type='submit' name='submit' value='submit'/>"
         "</form>"
         "</body>"
         "</html>";
      }
      
   }else if(omode=="check_authentication") {
      kingate_openid_message_t inm(mc);
      taiko_op_t OP(mc);
      opkele::openid_message_t oum;
      OP.check_authentication(oum,inm);
      content_type << "Content-Type: text/plain\r\n";
      oum.to_keyvalues(content);
      oum.to_keyvalues(clog);
   }else{
      taiko_op_t OP(mc);
      string idsel;
      if(mc.has_param("idsel"))
         idsel = "&idsel=idsel";
      content_type << "Content-Type: text/html\r\n";
      OP.cookie_header(header) << "X-XRDS-Location: " << get_self_url(mc) << "?op=xrds" << idsel << "\r\n";
      
      content <<
      "<html>"
      "<head>"
      "<title>test OP</title>"
      "<link rel='openid.server' href='" << get_self_url(mc) << "'/>"
      "</head>"
      "<body>"
      "test openid 2.0 endpoint"
      "<br/>"
      "<a href='" << get_self_url(mc) << "?op=xrds" << idsel << "'>XRDS document</a>"
      "<br/>"
      "<h1>" << message << "</h1>";
      
      if(OP.get_authorized()) {
         content <<
         "<br/>"
         "<a href='" << get_self_url(mc) << "?op=logout'>logout</a>";
      }else{
         content <<
         "<form method='post'>"
         "login "
         "<input type='hidden' name='op' value='login'/>"
         "<input type='username' name='username' value=''/>"
         "<input type='password' name='password' value=''/>"
         "<input type='submit' name='submit' value='submit'/>"
         "</form>";
      }
      content << "</body>";
   }
   
   mg_printf(conn,
             "%s"
             "%s"
             "%s"
             "Content-Length: %lu\r\n"  // Always set Content-Length
             "\r\n"
             "%s",
             status.tellp() > 0 ? status.str().c_str() : "HTTP/1.1 200 OK\r\n",
             content_type.str().c_str(),
             header.str().c_str(),
             (unsigned long)content.str().size(),
             content.str().c_str());
   
   // Mark as processed
   return MG_TRUE;
}

static int ev_handler(struct mg_connection *conn, enum mg_event event) {
   switch (event) {
      case MG_AUTH:
         return MG_TRUE;
      case MG_REQUEST:
         try {
            return handle_request(conn);
         } catch (taiko::exception& e) {
            mg_printf(conn,
                      "HTTP/1.1 200 OK\r\n"
                      "Content-Type: text/plain\r\n"
                      "Content-Length: %lu\r\n"  // Always set Content-Length
                      "\r\n"
                      "%s",
                      e.what().size(),
                      e.what().c_str());
            return MG_TRUE;
         }
      default: return MG_FALSE;
   }
}

int main(int argc, char** arg) {
   struct mg_server *server = NULL;
   
   users.parse_user_config();
   
   server = mg_create_server(NULL, ev_handler);
   mg_set_option(server, "listening_port", "8080");
   
   for (;;) {
      mg_poll_server(server, 1000);
   }
   mg_destroy_server(&server);
   
   return EXIT_SUCCESS;
}
