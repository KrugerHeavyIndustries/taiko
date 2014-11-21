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
#ifndef _TAIKO_MONGOOSE_CONNECTION_H_
#define _TAIKO_MONGOOSE_CONNECTION_H_

#include <map>

#include "exception.h"
#include "users.h"
#include "mongoose.h"
#include "cookies.h"
#include "utils.h"

namespace taiko {
   using namespace std;
   
class mongoose_connection_t {
    
public:
    
    typedef multimap<string, string> params_t;
    
    mg_connection* _connection;
    params_t get;
    params_t post;
    
    mongoose_connection_t(mg_connection* c)
    :   _connection(c) {
        
        string rm = get_request_method();
        if (rm == "GET") {
            string qs = get_query_string();
            if (!qs.empty()) {
                parse_query(qs, get);
            }
        } else if (rm == "POST") {
            string rb = get_request_body();
            if (!rb.empty()) {
                parse_query(rb, post);
            }
        }
    }
    
    string http_request_header(const string& hn) const {
        string mvn = "HTTP_";
        for(const char* p=hn.c_str();*p;p++) {
            if(*p=='-')
                mvn += '_';
            else
                mvn += toupper(*p);
        }
        return get_header(mvn);
    }
    
    string get_request_method() const {
        return _connection->request_method == NULL ? "" : _connection->request_method;
    }
    
    string get_query_string() const {
        return _connection->query_string == NULL ? "" : _connection->query_string;
    }
    
    string get_request_body() const {
         string post_data;
        if (_connection->content_len > 0) {
            post_data.assign(_connection->content, _connection->content_len);
            post_data.push_back('\0');
            return post_data;
        }
        return "";
    }
    
    string get_header(const string& mn) const {
        for(int i = 0; i < sizeof(_connection->http_headers) / sizeof(struct mg_connection::mg_header); ++i) {
            if (_connection->http_headers[i].name && mn == _connection->http_headers[i].name)
                return _connection->http_headers[i].value;
        }
       throw taiko::exception_notfound("no such meta-variable ('" + mn + "')");
    }
    
    kingate::cookie get_cookie(const string& name) {
        const size_t buf_len = 256;
        char buf[buf_len];
        const char* cookie = mg_get_header(_connection, "Cookie");
        if (cookie == NULL || mg_parse_header(cookie, name.c_str(), buf, buf_len) == 0)
           throw taiko::exception_notfound("no such cookie ('" + name + "')");
        return kingate::cookie(name, buf);
    }
    
    bool is_ssl() const {
        return is_proxy_ssl();
    }
   
    bool has_param(const string& p) const {
        return get.find(p) != get.end() || post.find(p) != post.end();
    }
    
    const string& get_param(const string& p) const {
        params_t::const_iterator i = get.find(p);
        if(i != get.end())
            return i->second;
        i = post.find(p);
        if(i != post.end())
            return i->second;
        throw taiko::exception_notfound("no such parameter");
    }
    
    int remote_port() const {
        return _connection->remote_port;
    }
   
private:
   
    bool is_proxy_ssl() const {
      try {
         return get_header("X-Forwarded-Proto") == "https";
      } catch (taiko::exception_notfound& e) {
         return false;
      }
    }
   
    static void parse_query(string q, params_t& p)  {
        while (!q.empty()) {
            string::size_type amp = q.find('&');
            string sub = (amp == string::npos) ? q : q.substr(0, amp);
            if (amp == string::npos)
                q.clear();
            else
                q.erase(0, amp + 1);
            string::size_type eq = sub.find('=');
            if (eq == string::npos) {
                p.insert(params_t::value_type("", url_decode(sub)));
            } else {
                p.insert(params_t::value_type(url_decode(sub.substr(0, eq)), url_decode(sub.substr(eq + 1))));
            }
        }
    }

};

} // namespace taiko

#endif // defined(_TAIKO_MONGOOSE_CONNECTION_H_)
