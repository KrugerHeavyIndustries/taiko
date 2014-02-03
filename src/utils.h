#ifndef _TAIKO_UTILS_H_
#define _TAIKO_UTILS_H_

#include <string>

namespace taiko {
   
class mongoose_connection_t;

std::string url_encode(const std::string& str);

std::string url_decode(const std::string& str);

std::string http_quoted_string(const std::string& str);

std::string http_quote(const std::string& str);

const std::string get_self_url(const mongoose_connection_t& mc);

} // namespace taiko

#endif // defined(_TAIKO_UTILS_H_)