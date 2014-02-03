#include "util.h"

#include <string>
#include <cstdlib>

#include "mongoose_connection.h"

using namespace std;

namespace taiko {
   
static const char* SAFE_CHARS = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                "abcdefghijklmnopqrstuvwxyz"
                                "0123456789_-";

static const char *quotible_chars =
"\001\002\003\004\005\006\007\010"
"\011\012\013\014\015\016\017\020"
"\021\022\023\024\025\026\027\030"
"\031\032\033\034\035\036\037\040"
"()<>@,;:\\\"/[]?={}" /* separator chars (except for SP and HT mentioned elsewhere */
"\177"
;

string url_encode(const string& str) {
   string rv = str;
   string::size_type screwed = 0;
   for (;;) {
      screwed = rv.find_first_not_of(SAFE_CHARS, screwed);
      if (screwed == string::npos) 
         break;

      while (screwed < rv.length() && !strchr(SAFE_CHARS, rv.at(screwed))) {
         char danger = rv.at(screwed);
         if (danger == ' ') { 
            rv.replace(screwed++, 1, 1, '+');
         } else {
            static char tmp[4] = {'%',0,0,0};
            snprintf(&tmp[1],3,"%02X",0xFF&(int)danger);
            rv.replace(screwed,1,tmp,3);
            screwed+=3;
         }
      }
   }
   return rv;
}

string url_decode(const string& str) {
   string rv = str;
   string::size_type unscrewed = 0;
   for(;;) {
      unscrewed = rv.find_first_of("%+",unscrewed);
      if(unscrewed == string::npos)
         break;
      if(rv.at(unscrewed)=='+') {
         rv.replace(unscrewed++,1,1,' ');
      }else{
         if((rv.length()-unscrewed)<3)
            throw exception("unexpected length");
         // XXX: ensure it's hex?
         int danger = strtol(rv.substr(unscrewed+1,2).c_str(),NULL,16);
         rv.replace(unscrewed,3,1,danger);
         unscrewed++;
      }
  }
  return rv;
}

string http_quoted_string(const string& str) {
    string rv = str;
    //string::size_type sp=0;
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
