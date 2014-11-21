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
#include "cookies.h"
#include "utils.h"

namespace kingate {
   
   using namespace taiko;

    /*
     * RFC 2109:
     *    av-pairs        =       av-pair *(";" av-pair)
     *    av-pair         =       attr ["=" value]        ; optional value
     *    attr            =       token
     *    value           =       word
     *    word            =       token | quoted-string
     */

    /* RFC 2109:
     *
     * The origin server effectively ends a session by sending the client a
     * Set-Cookie header with Max-Age=0.
     *
     * An origin server may include multiple Set-Cookie headers in a response.
     * Note that an intervening gateway could fold multiple such headers into a
     * single header.
     *
     *
     *    set-cookie      =       "Set-Cookie:" cookies
     *    cookies         =       1#cookie
     *    cookie          =       NAME "=" VALUE *(";" cookie-av)
     *    NAME            =       attr
     *    VALUE           =       value
     *    cookie-av       =       "Comment" "=" value
     *                    |       "Domain" "=" value
     *                    |       "Max-Age" "=" value
     *                    |       "Path" "=" value
     *                    |       "Secure"
     *                    |       "Version" "=" 1*DIGIT
     *
     *
     *    The origin server should send the following additional HTTP/1.1
     *    response headers, depending on circumstances:
     *
     *    * To suppress caching of the Set-Cookie header: Cache-control: no-
     *      cache="set-cookie".
     *
     *    and one of the following:
     *
     *    * To suppress caching of a private document in shared caches: Cache-
     *      control: private.
     *
     *    * To allow caching of a document and require that it be validated
     *      before returning it to the client: Cache-control: must-revalidate.
     *
     *    * To allow caching of a document, but to require that proxy caches
     *      (not user agent caches) validate it before returning it to the
     *      client: Cache-control: proxy-revalidate.
     *
     *    * To allow caching of a document and request that it be validated
     *      before returning it to the client (by "pre-expiring" it):
     *      Cache-control: max-age=0.  Not all caches will revalidate the
     *      document in every case.
     *
     *    HTTP/1.1 servers must send Expires: old-date (where old-date is a
     *    date long in the past) on responses containing Set-Cookie response
     *    headers unless they know for certain (by out of band means) that
     *    there are no downsteam HTTP/1.0 proxies.  HTTP/1.1 servers may send
     *    other Cache-Control directives that permit caching by HTTP/1.1
     *    proxies in addition to the Expires: old-date directive; the Cache-
     *    Control directive will override the Expires: old-date for HTTP/1.1
     *    proxies.
     *
     */

    void cookie::_set_string(const string& p,const string& v) {
	(*this)[p]=v;
    }

    void cookie::set_comment(const string& c) {
	_set_string("comment",c);
    }
    void cookie::set_domain(const string& d) {
	_set_string("domain",d);
    }
    void cookie::set_max_age(const string& ma) {
	_set_string("max-age",ma);
    }
    void cookie::set_path(const string& p) {
	_set_string("path",p);
    }
    void cookie::set_secure(bool s) {
	if(s)
	    _set_string("secure","");
	else
	    erase("secure");
    }

    void cookie::set_expires(const string& e) {
	(*this)["expires"] = e;
    }

    const string& cookie::_get_string(const string& s) const {
	const_iterator i = find(s);
	if(i==end())
        throw exception();
	    //throw exception_notfound(CODEPOINT,"No parameter set");
	return i->second;
    }

    const string& cookie::get_comment() const {
	return _get_string("comment");
    }
    const string& cookie::get_domain() const {
	return _get_string("domain");
    }
    const string& cookie::get_max_age() const {
	return _get_string("max-age");
    }
    const string& cookie::get_path() const {
	return _get_string("path");
    }
    bool cookie::get_secure() const {
	return find("secure")!=end();
    }

    const string& cookie::get_expires() const {
	return _get_string("expires");
    }

    bool cookie::has_comment() const {
	return find("comment")!=end();
    }
    bool cookie::has_domain() const {
	return find("domain")!=end();
    }
    bool cookie::has_max_age() const {
	return find("max-age")!=end();
    }
    bool cookie::has_path() const {
	return find("path")!=end();
    }

    bool cookie::has_expires() const {
	return find("expires")!=end();
    }

    void cookie::unset_comment() {
	erase("comment");
    }
    void cookie::unset_domain() {
	erase("domain");
    }
    void cookie::unset_max_age() {
	erase("max-age");
    }
    void cookie::unset_path() {
	erase("path");
    }

    void cookie::unset_expires() {
	erase("expires");
    }

    string cookie::set_cookie_header_rfc2109() const {
	string rv = name + "=" + http_quoted_string(value);
	for(const_iterator i=begin();i!=end();++i) {
	    if(i->first=="secure") {
		rv += "; secure";
	    }else{
		rv += "; "+i->first+"="+http_quote(i->second);
	    }
	}
	rv += "; Version=1";
	return rv;
    }

    string cookie::set_cookie_header() const {
	string rv = name + "=" + value;
	for(const_iterator i=begin();i!=end();++i) {
	    if(i->first=="secure") {
		rv += "; secure";
	    }else{
		rv += "; "+i->first+"="+i->second;
	    }
	}
	return rv;
    }

    void cookies_t::set_cookie(const cookie& c) {
	insert(value_type(c.get_name(),c));
    }

    bool cookies_t::has_cookie(const key_type& n) const {
	return find(n)!=end();
    }

    const cookie& cookies_t::get_cookie(const key_type& n) const {
	const_iterator i=find(n);
	if(i==end())
        throw exception();
	    //throw exception_notfound(CODEPOINT,"No cookie with such name found");
	return i->second;
    }

    cookie& cookies_t::get_cookie(const key_type& n) {
	iterator i=find(n);
	if(i==end())
        throw exception();
	    //throw exception_notfound(CODEPOINT,"No cookie with such name found");
	return i->second;
    }

    void cookies_t::parse_cookies(const string& s) {
	string str = s;
	while(!str.empty()) {
	    string::size_type sc = str.find(';');
	    string s;
	    if(sc==string::npos) {
		s = str;
		str.erase();
	    }else{
		s = str.substr(0,sc);
		str.erase(0,sc+1);
	    }
	    string::size_type nsp=s.find_first_not_of(" \t");
	    if((nsp!=string::npos) && nsp)
		s.erase(0,nsp);
	    string::size_type eq=s.find('=');
	    if(eq==string::npos)
		continue;
	    string n = s.substr(0,eq);
	    s.erase(0,eq+1);
	    nsp = n.find_last_not_of(" \t");
	    n.erase(nsp+1);
	    nsp = s.find_first_not_of(" \t");
	    string v;
	    if(nsp!=string::npos)
		v = s.substr(nsp);
	    else
		v = s;
	    nsp = v.find_last_not_of(" \t");
	    if(nsp==string::npos)
		v.erase();
	    else
		v.erase(nsp+1);
	    set_cookie(cookie(n,v));
	}
    }

}
