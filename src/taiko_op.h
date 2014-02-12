//
//  taiko_op.h
//  openidd
//
//  Created by Chris Kruger on 3/02/2014.
//
//

#ifndef _TAIKO_TAIKO_OP_
#define _TAIKO_TAIKO_OP_

#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <opkele/verify_op.h>
#include <opkele/exception.h>
#include <opkele/association.h>
#include <opkele/util.h>
#include <opkele/uris.h>
#include <opkele/sreg.h>

#include "mongoose.h"
#include "sqlite.h"
#include "mongoose_connection.h"
#include "uuid/uuid.h"

namespace taiko {
   
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
      }else {
         sqlite3_free_table(resp);
      }
   }
};

class taiko_op_t : public opkele::verify_OP {
public:
   mongoose_connection_t& mc;
   opdb_t db;
   kingate::cookie htc;
   
	taiko_op_t(mongoose_connection_t& c)
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
   
   void set_username(const string& username) {
      sqlite3_mem_t<char*>
		S = sqlite3_mprintf(
                          "UPDATE ht_sessions"
                          " SET username=%Q"
                          " WHERE hts_id=%Q",
                          username.c_str(),htc.get_value().c_str());
      db.exec(S);
   }
   
   string get_username() {
      sqlite3_mem_t<char*>
		S = sqlite3_mprintf(
                          "SELECT username"
                          " FROM ht_sessions"
                          " WHERE hts_id=%Q",
                          htc.get_value().c_str());
      sqlite3_table_t T; int nr,nc;
      db.get_table(S,T,&nr,&nc);
      assert(nr==1); assert(nc=1);
      return T.get(1,0,nc);
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

} // namespace taiko

#endif /* defined(_TAIKO_TAIKO_OP_) */
