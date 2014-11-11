#include "utils.h"

#include <string>
#include <cstring>
#include <cstdlib>

#include "mongoose_connection.h"

using namespace std;

namespace taiko {
   
static const char *quotible_chars =
   "\001\002\003\004\005\006\007\010"
   "\011\012\013\014\015\016\017\020"
   "\021\022\023\024\025\026\027\030"
   "\031\032\033\034\035\036\037\040"
   "()<>@,;:\\\"/[]?={}" /* separator chars (except for SP and HT mentioned elsewhere */
   "\177";

   
#define ENCODE_MAX_LENGTH 2048
   
string url_encode(const string& str) {
   char encoded[ENCODE_MAX_LENGTH];
   mg_url_encode(str.c_str(), str.length(), encoded, ENCODE_MAX_LENGTH);
   return string(encoded);
}

string url_decode(const string& str) {
   char decoded[ENCODE_MAX_LENGTH];
   mg_url_decode(str.c_str(), str.length(), decoded, ENCODE_MAX_LENGTH, false);
   return decoded;
}

string http_quoted_string(const string& str) {
    string rv = str;
    for(string::size_type q=rv.find('"');(q=rv.find('"',q))!=string::npos;q+=2)
        rv.insert(q,1,'\\');
    rv.insert(0,1,'"');
    rv += '"';
    return rv;
}

string http_quote(const string& str) {
    if(str.find_first_of(quotible_chars)==string::npos)
        return str;
    return http_quoted_string(str);
}

const string get_self_url(const mongoose_connection_t& mc) {
   string rv = mc.is_ssl() ? "https://" : "http://";
   rv += mc.get_header("Host");
   string::size_type q = rv.find(':');
   if (q == string::npos) {
      rv += ':';
      rv += mc.is_ssl() ? "443" : "80";
   }
   return rv;
}
   
} // namespace taiko
