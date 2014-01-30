//
//  mongoose_connection.h
//  openidserver
//
//  Created by Chris Kruger on 27/01/13.
//
//

#ifndef MONGOOSE_CONNECTION_H
#define MONGOOSE_CONNECTION_H

#include "exception.h"
#include "users.h"

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
        const char* rm = mg_get_request_info(_connection)->request_method;
        return rm == NULL ? "" : rm;
    }
    
    string get_query_string() const {
        const char* qs = mg_get_request_info(_connection)->query_string;
        return qs == NULL ? "" : qs;
    }
    
    string get_request_body() const {
        char post_data[1024];
        int post_data_len = mg_read(_connection, post_data, sizeof(post_data));
        if (post_data_len > 0) {
            post_data[post_data_len + 1] = '\0';
            return post_data;
        }
        return "";
    }
    
    string get_header(const string& mn) const {
        mg_request_info* info = mg_get_request_info(_connection);
        
        for(int i = 0; i < sizeof(info->http_headers) / sizeof(struct mg_request_info::mg_header); ++i) {
            if (info->http_headers[i].name && mn == info->http_headers[i].name)
                return info->http_headers[i].value;
        }
       throw taiko::exception_notfound("no such meta-variable ('" + mn + "')");
    }
    
    kingate::cookie get_cookie(const string& name) {
        const size_t buf_len = 256;
        char buf[buf_len];
        if (mg_get_cookie(_connection, name.c_str(), buf, buf_len) == 0)
           throw taiko::exception_notfound("no such cookie ('" + name + "')");
        return kingate::cookie(name, buf);
    }
    
    bool is_ssl() const {
        return mg_get_request_info(_connection)->is_ssl > 0;
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
        return mg_get_request_info(_connection)->remote_port;
    }
   
private:
    
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


#endif
