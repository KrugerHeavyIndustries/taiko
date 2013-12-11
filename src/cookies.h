#ifndef __KINGATE_COOKIES_H
#define __KINGATE_COOKIES_H

#include <string>
#include <map>
#include <ostream>

/**
 * @file
 * @brief cookies-related classes.
 */

namespace kingate {
    using namespace std;

    /**
     * Class, holding the cookie with parameters.
     */
    class cookie : public map<string,string> {
	public:
	    /**
	     * Cookie name.
	     */
	    string name;
	    /**
	     * Cookie value.
	     */
	    string value;

	    cookie() { }
	    /**
	     * @param n cookie name.
	     * @param v cookie value.
	     */
	    cookie(const string& n,const string& v)
		: name(n), value(v) { }

	    /**
	     * set cookie parameter.
	     * @param p parameter name.
	     * @param v parameter value.
	     * @see _get_string()
	     */
	    void _set_string(const string& p,const string& v);
	   
	    /**
	     * @param n cookie name.
	     * @see get_name()
	     */
	    void set_name(const string& n) { name = n; }
	    /**
	     * @param v cookie value.
	     * @see set_value()
	     */
	    void set_value(const string& v) { value = v; }
	    /**
	     * @param c coomment.
	     * @see get_comment()
	     * @see has_comment()
	     * @see unset_comment()
	     */
	    void set_comment(const string& c);
	    /**
	     * @param d domain.
	     * @see get_domain()
	     * @see has_domain()
	     * @see unset_domain()
	     */
	    void set_domain(const string& d);
	    /**
	     * @param ma max-age.
	     * @see get_max_age()
	     * @see has_max_age()
	     * @see unset_max_age()
	     */
	    void set_max_age(const string& ma);
	    /**
	     * @param p path.
	     * @see get_path()
	     * @see has_path()
	     * @see unset_path()
	     */
	    void set_path(const string& p);
	    /**
	     * set cookie security.
	     * @param s true if secure.
	     * @see get_secure()
	     * @see is_secure()
	     */
	    void set_secure(bool s);

	    /**
	     * @param e expiration time.
	     * @see get_expires()
	     * @see has_expires()
	     * @see unset_expires()
	     */
	    void set_expires(const string& e);

	    /**
	     * get cookie parameter.
	     * @param p parameter name.
	     * @return parameter value.
	     * @see _set_string()
	     */
	    const string& _get_string(const string& p) const;

	    /**
	     * @return cookie name.
	     * @see set_name()
	     */
	    const string& get_name() const { return name; }
	    /**
	     * @return cookie value.
	     * @see set_value()
	     */
	    const string& get_value() const { return value; }
	    /**
	     * @return cookie comment.
	     * @see set_comment()
	     * @see has_comment()
	     * @see unset_comment()
	     */
	    const string& get_comment() const;
	    /**
	     * @return cookie domain.
	     * @see set_domain()
	     * @see has_domain()
	     * @see unset_domain()
	     */
	    const string& get_domain() const;
	    /**
	     * @return cookie max-age.
	     * @see set_max_age()
	     * @see has_max_age()
	     * @see unset_max_age()
	     */
	    const string& get_max_age() const;
	    /**
	     * @return cookie path.
	     * @see set_path()
	     * @see has_path()
	     * @see unset_path()
	     */
	    const string& get_path() const;
	    /**
	     * @return cookie security.
	     * @see is_secure()
	     * @see set_secure()
	     */
	    bool get_secure() const;
	    /**
	     * @return cookie security.
	     * @see get_secure()
	     * @see set_secure()
	     */
	    bool is_secure() const { return get_secure(); }

	    /**
	     * @return cookie expiration time.
	     * @see set_expires()
	     * @see has_expires()
	     * @see unset_expires()
	     */
	    const string& get_expires() const;

	    /**
	     * @return true if cookie has comment.
	     * @see set_comment()
	     * @see get_comment()
	     * @see unset_comment()
	     */
	    bool has_comment() const;
	    /**
	     * @return true if cookie has domain.
	     * @see set_domain()
	     * @see get_domain()
	     * @see unset_domain()
	     */
	    bool has_domain() const;
	    /**
	     * @return true if cookie has max-age.
	     * @see set_max_age()
	     * @see get_max_age()
	     * @see unset_max_age()
	     */
	    bool has_max_age() const;
	    /**
	     * @return true if cookie has path.
	     * @see set_path()
	     * @see get_path()
	     * @see unset_path()
	     */
	    bool has_path() const;

	    /**
	     * @return true if cookie has expiration time.
	     * @see set_expires()
	     * @see get_expires()
	     * @see unset_expires()
	     */
	    bool has_expires() const;

	    /**
	     * rid cookie of comment.
	     * @see set_comment()
	     * @see get_comment()
	     * @see has_comment()
	     */
	    void unset_comment();
	    /**
	     * rid cookie of domain.
	     * @see set_domain()
	     * @see get_domain()
	     * @see has_domain()
	     */
	    void unset_domain();
	    /**
	     * rid cookie of max-age.
	     * @see set_max_age()
	     * @see get_max_age()
	     * @see has_max_age()
	     */
	    void unset_max_age();
	    /**
	     * rid cookie of path.
	     * @see set_path()
	     * @see get_path()
	     * @see has_path()
	     */
	    void unset_path();

	    /**
	     * rid cookie of expiration time.
	     * @see set_expires()
	     * @see get_expires()
	     * @see has_expires()
	     */
	    void unset_expires();

	    /**
	     * render the 'Set-Cookie' HTTP header according to RFC2109.
	     * Absolutely useless, only works with lynx.
	     * @return the rendered header content.
	     */
	    string set_cookie_header_rfc2109() const;
	    /**
	     * render the 'Set-Cookie' header according to the early vague
	     * netscape specs and common practice.
	     * @return the rendered header content.
	     */
	    string set_cookie_header() const;
    };

    /**
     * Cookies container class.
     */
    class cookies_t : public multimap<string,cookie> {
	public:

	    cookies_t() { }
	    /**
	     * @param s 'Cookie:' HTTP header contents to parse.
	     */
	    cookies_t(const string& s) { parse_cookies(s); }

	    /**
	     * @param c cookie to set.
	     */
	    void set_cookie(const cookie& c);
	    /**
	     * @param n cookie name to remove.
	     */
	    void unset_cookie(const key_type& n) { erase(n); }
	    /**
	     * @param n cookie name.
	     * @return true if exists.
	     */
	    bool has_cookie(const key_type& n) const;
	    /**
	     * Return the named cookie if one exists.
	     * @param n cookie name.
	     * @return const reference to cookie object.
	     */
	    const cookie& get_cookie(const key_type& n) const;
	    /**
	     * Return the named cookie if one exists.
	     * @param n cookie name.
	     * @return reference to cookie object.
	     */
	    cookie& get_cookie(const key_type& n);

	    /**
	     * @param s HTTP 'Cookie' header content.
	     */
	    void parse_cookies(const string& s);
    };
}

#endif /* __KINGATE_COOKIES_H */
