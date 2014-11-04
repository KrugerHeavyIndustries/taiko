/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

/* defined if cURL is not to verify cert/host */
/* #undef DISABLE_CURL_SSL_VERIFYHOST */

/* defined if cURL is not to verify cert validity */
/* #undef DISABLE_CURL_SSL_VERIFYPEER */

/* defined if abi::__cxa_demangle is available */
#define HAVE_DEMANGLE /**/

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* defined in presence of konforka library */
/* #undef HAVE_KONFORKA */

/* Define to 1 if you have a functional curl library. */
#define HAVE_LIBCURL 1

/* defined in presence of libuuid */
#define HAVE_LIBUUID /**/

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <tidy.h> header file. */
/*#if defined(__linux__)
#define HAVE_TIDY_H
#endif 
*/

/* Define to 1 if you have the <tidy/tidy.h> header file. */
#if defined(__APPLE__) || defined(__linux__)
#define HAVE_TIDY_TIDY_H 1
#endif 

/* Define to 1 if you have the `timegm' function. */
#define HAVE_TIMEGM 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Defined if libcurl supports AsynchDNS */
#define LIBCURL_FEATURE_ASYNCHDNS 1

/* Defined if libcurl supports IDN */
/* #undef LIBCURL_FEATURE_IDN */

/* Defined if libcurl supports IPv6 */
#define LIBCURL_FEATURE_IPV6 1

/* Defined if libcurl supports KRB4 */
/* #undef LIBCURL_FEATURE_KRB4 */

/* Defined if libcurl supports libz */
#define LIBCURL_FEATURE_LIBZ 1

/* Defined if libcurl supports NTLM */
#define LIBCURL_FEATURE_NTLM 1

/* Defined if libcurl supports SSL */
#define LIBCURL_FEATURE_SSL 1

/* Defined if libcurl supports SSPI */
/* #undef LIBCURL_FEATURE_SSPI */

/* Defined if libcurl supports DICT */
#define LIBCURL_PROTOCOL_DICT 1

/* Defined if libcurl supports FILE */
#define LIBCURL_PROTOCOL_FILE 1

/* Defined if libcurl supports FTP */
#define LIBCURL_PROTOCOL_FTP 1

/* Defined if libcurl supports FTPS */
#define LIBCURL_PROTOCOL_FTPS 1

/* Defined if libcurl supports HTTP */
#define LIBCURL_PROTOCOL_HTTP 1

/* Defined if libcurl supports HTTPS */
#define LIBCURL_PROTOCOL_HTTPS 1

/* Defined if libcurl supports LDAP */
#define LIBCURL_PROTOCOL_LDAP 1

/* Defined if libcurl supports TELNET */
#define LIBCURL_PROTOCOL_TELNET 1

/* Defined if libcurl supports TFTP */
#define LIBCURL_PROTOCOL_TFTP 1

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#define LT_OBJDIR ".libs/"

/* deprecated function attribute */
#define OPKELE_DEPRECATE __attribute__((deprecated))

/* defined in presence of konforka library */
/* #undef OPKELE_HAVE_KONFORKA */

/* source directory */
#define OPKELE_SRC_DIR "/Users/chriskruger/Development/libopkele"

/* Name of package */
#define PACKAGE "libopkele"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "libopkele-bugs@klever.net"

/* Define to the full name of this package. */
#define PACKAGE_NAME "libopkele"

/* more or less precise source tree version */
#define PACKAGE_SRC_VERSION "2.0.4-2-g824440e-dirty"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "libopkele 2.0.4"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "libopkele"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "2.0.4"

/* defined if we want to adhere to Postel's Law */
#define POSTELS_LAW /**/

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Version number of package */
#define VERSION "2.0.4"

/* XRI proxy resolver URL */
#define XRI_PROXY_URL "https://xri.net/"

/* Define curl_free() as free() if our version of curl lacks curl_free. */
/* #undef curl_free */
