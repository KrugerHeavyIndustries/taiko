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
#include "sqlite.h"
#include "utils.h"
#include "cookies.h"
#include "mongoose.h"
#include "kingate_openid_message.h"

#include "auth_bsd.h" 
#include "users.h"

#include "taiko_op.h"

#define _PATH_TAIKODCONF "/etc/taikod.conf" 

static const char* OK = "";

using namespace std;
using namespace taiko;

users_t users;

static void* handle_new_request(struct mg_connection* conn) {
   
   mongoose_connection_t mc(conn);
   
   ostringstream status;
   ostringstream header;
   ostringstream content_type;
   ostringstream content;

   if (!mc.is_ssl())
      throw taiko::exception("SSL connection required");
      
   string op;
   if (mc.has_param("op"))
      op = mc.get_param("op");
   
   string message;
   if(op=="login") {
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
   }else if(op=="logout") {
      taiko_op_t OP(mc);
      OP.set_authorized(false);
      op.clear();
      message = "logged out";
   }
   string omode;
   if (mc.has_param("openid.mode"))
      omode = mc.get_param("openid.mode");
   if(op=="xrds") {
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
   }else if(op=="id_res" || op=="cancel") {
      kingate_openid_message_t inm(mc);
      taiko_op_t OP(mc);
      if (mc.get_param("hts_id") != OP.htc.get_value())
         throw taiko::exception("toying around, huh?");
      opkele::sreg_t sreg;
      OP.checkid_(inm, sreg);
      OP.cookie_header(header);
      opkele::openid_message_t om;
      if(op=="id_res") {
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
             content.str().size(),
             content.str().c_str());
   
   // Mark as processed
   return static_cast<void*>(&OK);
}

static void* callback(enum mg_event event, struct mg_connection *conn) {
   if (event == MG_NEW_REQUEST) {
      try {
         return handle_new_request(conn);
      } catch (taiko::exception& e) {
         mg_printf(conn,
                   "HTTP/1.1 200 OK\r\n"
                   "Content-Type: text/plain\r\n"
                   "Content-Length: %lu\r\n"  // Always set Content-Length
                   "\r\n"
                   "%s",
                   e.what().size(),
                   e.what().c_str());
         return static_cast<void*>(&OK);
      }
   } else {
      return NULL;
   }
}

int main(int argc, char** arg) {
   struct mg_context *ctx;
   const char *options[] = {"listening_ports", "8080", NULL};
   
   users.parse_user_config();
   
   ctx = mg_start(&callback, NULL, options);
   getchar();  // Wait until user hits "enter"
   mg_stop(ctx);

   return 0;
}
