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

static const char* OK = "";

using namespace std;

static const string get_self_url(const mongoose_connection_t& mc) {
    string rv = mc.is_ssl() ? "https://" : "http://";
    rv += mc.get_header("Host");
    string::size_type q = rv.find(':');
    if (q == string::npos) {
        rv += ':';
        rv += mc.is_ssl() ? "443" : "80";
    }
    return rv;
}

class opdb_t : public sqlite3_t {
public:
    opdb_t()
    : sqlite3_t("/tmp/OP.db") {
        assert(_D);
        char **resp; int nr,nc; char *errm;
        if(sqlite3_get_table(_D, "SELECT a_op FROM assoc LIMIT 0",
                             &resp,&nr,&nc,&errm)!=SQLITE_OK) {
            extern const char *__OP_db_bootstrap;
            //DOUT_("Bootstrapping DB");
            if(sqlite3_exec(_D,__OP_db_bootstrap,NULL,NULL,&errm)!=SQLITE_OK)
                throw opkele::exception(OPKELE_CP_ string("Failed to boostrap SQLite database: ")+errm);
        }else
            sqlite3_free_table(resp);
    }   
};

class example_op_t : public opkele::verify_OP {
    public:
    mongoose_connection_t& mc;
    opdb_t db;
	 kingate::cookie htc;

	example_op_t(mongoose_connection_t& c)
    : mc(c) {
	    try {
		htc = mc.get_cookie("htop_session");
		sqlite3_mem_t<char*> S = sqlite3_mprintf(
			"SELECT 1 FROM ht_sessions WHERE hts_id=%Q",
			htc.get_value().c_str());
		sqlite3_table_t T; int nr,nc;
		db.get_table(S,T,&nr,&nc);
		if(nr<1)
         throw taiko::exception("forcing cookie generation");
	    }catch(taiko::exception& kenf) {
		uuid_t uuid; uuid_generate(uuid);
		htc = kingate::cookie("htop_session",opkele::util::encode_base64(uuid,sizeof(uuid)));
		sqlite3_mem_t<char*> S = sqlite3_mprintf(
			"INSERT INTO ht_sessions (hts_id) VALUES (%Q)",
			htc.get_value().c_str());
		db.exec(S);
	    }
	}

	void set_authorized(bool a) {
	    sqlite3_mem_t<char*>
		S = sqlite3_mprintf(
			"UPDATE ht_sessions"
			" SET authorized=%d"
			" WHERE hts_id=%Q",
			(int)a,htc.get_value().c_str());
	    db.exec(S);
	}
	bool get_authorized() {
	    sqlite3_mem_t<char*>
		S = sqlite3_mprintf(
			"SELECT authorized"
			" FROM ht_sessions"
			" WHERE hts_id=%Q",
			htc.get_value().c_str());
	    sqlite3_table_t T; int nr,nc;
	    db.get_table(S,T,&nr,&nc);
	    assert(nr==1); assert(nc=1);
	    return opkele::util::string_to_long(T.get(1,0,nc));
	}

	ostream& cookie_header(ostream& o) const {
	    o << "Set-Cookie: " << htc.set_cookie_header() << "\r\n";
	    return o;
	}

	opkele::assoc_t alloc_assoc(const string& type,size_t klength,bool sl) {
	    uuid_t uuid; uuid_generate(uuid);
	    string a_handle = opkele::util::encode_base64(uuid,sizeof(uuid));
	    opkele::secret_t a_secret;
	    generate_n(
		    back_insert_iterator<opkele::secret_t>(a_secret),klength,
		    rand );
	    string ssecret; a_secret.to_base64(ssecret);
	    time_t now = time(0);
	    int expires_in = sl?3600*2:3600*24*7*2;
	    sqlite3_mem_t<char*>
		S = sqlite3_mprintf(
			"INSERT INTO assoc"
			" (a_handle,a_type,a_ctime,a_etime,a_secret,a_stateless)"
			" VALUES ("
			"  %Q,%Q,datetime('now'),"
			"  datetime('now','+%d seconds'),"
			"  %Q,%d );",
			a_handle.c_str(), type.c_str(),
			expires_in,
			ssecret.c_str(), sl );
	    db.exec(S);
	    return opkele::assoc_t(new opkele::association(
			"",
			a_handle, type, a_secret,
			now+expires_in, sl ));
	}

	opkele::assoc_t retrieve_assoc(const string& h) {
	    sqlite3_mem_t<char*>
		S = sqlite3_mprintf(
			"SELECT"
			"  a_handle,a_type,a_secret,a_stateless,"
			"  strftime('%%s',a_etime) AS a_etime,"
			"  a_itime"
			" FROM assoc"
			" WHERE a_handle=%Q AND a_itime IS NULL"
			"  AND datetime('now') < a_etime"
			" LIMIT 1",
			h.c_str() );
	    sqlite3_table_t T;
	    int nr,nc;
	    db.get_table(S,T,&nr,&nc);
	    if(nr<1)
		throw opkele::failed_lookup(OPKELE_CP_
			"couldn't retrieve valid unexpired assoc");
	    assert(nr==1); assert(nc==6);
	    opkele::secret_t secret; opkele::util::decode_base64(T.get(1,2,nc),secret);
	    return opkele::assoc_t(new opkele::association(
			"", h, T.get(1,1,nc), secret,
			strtol(T.get(1,4,nc),0,0),
			strtol(T.get(1,3,nc),0,0) ));
	}

	string& alloc_nonce(string& nonce) {
	    uuid_t uuid; uuid_generate(uuid);
	    nonce += opkele::util::encode_base64(uuid,sizeof(uuid));
	    sqlite3_mem_t<char*>
		S = sqlite3_mprintf(
			"INSERT INTO nonces"
			" (n_once) VALUES (%Q)",
			nonce.c_str() );
	    db.exec(S);
	    return nonce;
	}
	bool check_nonce(const string& nonce) {
	    sqlite3_mem_t<char*>
		S = sqlite3_mprintf(
			"SELECT 1"
			" FROM nonces"
			" WHERE n_once=%Q AND n_itime IS NULL",
			nonce.c_str());
	    sqlite3_table_t T;
	    int nr,nc;
	    db.get_table(S,T,&nr,&nc);
	    return nr>=1;
	}
	void invalidate_nonce(const string& nonce) {
	    sqlite3_mem_t<char*>
		S = sqlite3_mprintf(
			"UPDATE nonces"
			" SET n_itime=datetime('now')"
			" WHERE n_once=%Q",
			nonce.c_str());
	    db.exec(S);
	}

	const string get_op_endpoint() const {
	    return get_self_url(mc);
	}

};

static void* callback(enum mg_event event, struct mg_connection *conn) {

    if (event == MG_NEW_REQUEST) {
          
         mongoose_connection_t mc(conn);
       
         ostringstream status;
         ostringstream header;
         ostringstream content_type;
         ostringstream content;
         
         string op;
         if (mc.has_param("op"))
             op = mc.get_param("op");
       
         string message;
         if(op=="login") {
            example_op_t OP(mc);
            string username = mc.get_param("username");
            string password = mc.get_param("password");
            string memoized_params = mc.get_param("memoized_params");
           if (check_auth(username.c_str(), password.c_str()) == STATUS_OK) {
               OP.set_authorized(true);
               op.clear();
               status << "HTTP/1.1 302 Going back to OP with checkid_setup after successful login\r\n";
               header << "Location: " << get_self_url(mc) << "?" << memoized_params << "\r\n";
               OP.cookie_header(header);
           } else {
              throw opkele::exception(OPKELE_CP_ "wrong password");
           }
        }else if(op=="logout") {
            example_op_t OP(mc);
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
           example_op_t OP(mc);
           if (mc.get_param("hts_id") != OP.htc.get_value())
              throw opkele::exception(OPKELE_CP_ "toying around, huh?");
           opkele::sreg_t sreg;
           OP.checkid_(inm, sreg);
           OP.cookie_header(header);
           opkele::openid_message_t om;
           if(op=="id_res") {
              if(!OP.get_authorized())
                 throw opkele::exception(OPKELE_CP_ "not logged in");
              if(OP.is_id_select()) {
                 OP.select_identity( get_self_url(mc), get_self_url(mc) );
           }
           sreg.set_field(opkele::sreg_t::field_nickname,"anonymous");
           sreg.set_field(opkele::sreg_t::field_fullname,"Ann'O'Nymus");
           //sreg.set_field(opkele::sreg_t::field_gender,"F");
           sreg.setup_response();
               status << "HTTP/1.1 302 Going back to RP with id_res\r\n";
               header <<
                  "Location: " << OP.id_res(om, sreg).append_query(OP.get_return_to())
                  << "\r\n";
            }else{
               status << "HTTP/1.1 302 Going back to RP with cancel\r\n";
               header <<
                  "Location: " << OP.cancel(om).append_query(OP.get_return_to())
                  << "\r\n";
            }
            om.to_keyvalues(clog);
        }else if(omode=="associate") {
            const kingate_openid_message_t inm(mc);
            opkele::openid_message_t oum;
            example_op_t OP(mc);
            OP.associate(oum,inm);
            content_type << "Content-Type: text/plain\r\n";
            oum.to_keyvalues(content);
        }else if(omode=="checkid_setup") {
            kingate_openid_message_t inm(mc);
            example_op_t OP(mc);
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
               OP.select_identity( get_self_url(mc), get_self_url(mc) );
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
              "<input type='hidden' name='memoized_params' value='" << inm.query_string()
              //<< "openid.assoc_handle=" << "kfCHgLfuSZmt4E62t4%2BzBw%3D%3D" << "&"
              //<< "openid.claimed_id=" << opkele::util::attr_escape(OP.get_claimed_id()) << "&"
              //<< "openid.identity=" << opkele::util::attr_escape(OP.get_identity()) << "&"
              //<< "openid.mode=checkid_setup" << "&"
              //<< "openid.ns=" << opkele::util::attr_escape(OIURI_OPENID20) << "&"
              //<< "openid.realm=" << opkele::util::attr_escape(OP.get_realm()) << "&"
              //<< "openid.return_to=" << opkele::util::attr_escape(OP.get_return_to())
              << "'/>" <<
              "<input type='username' name='username' value=''/>"
              "<input type='password' name='password' value=''/>"
              "<input type='submit' name='submit' value='submit'/>"
              "</form>"
              "</body>"
              "</html>";
           }

        }else if(omode=="check_authentication") {
            kingate_openid_message_t inm(mc);
            example_op_t OP(mc);
            opkele::openid_message_t oum;
            OP.check_authentication(oum,inm);
            content_type << "Content-Type: text/plain\r\n";
            oum.to_keyvalues(content);
            oum.to_keyvalues(clog);
        }else{
            example_op_t OP(mc);
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
    } else {   
        return NULL;
    }
}

int main(int argc, char** arg) {
    struct mg_context *ctx;
    const char *options[] = {"listening_ports", "8080", NULL};

    ctx = mg_start(&callback, NULL, options);
    getchar();  // Wait until user hits "enter"
    mg_stop(ctx);

    return 0;
}
