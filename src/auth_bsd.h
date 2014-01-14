/* =======================================================================
 * Copyright 1996, Jan D. Wolter and Steven R. Weiss, All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. The names of the authors must not be used to endorse or
 *    promote products derived from this software without prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY
 * EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * =======================================================================
 */

#ifndef _AUTH_BSD_H_
#define _AUTH_BSD_H_


#define STATUS_OK         0   /* Valid Login */
#define STATUS_UNKNOWN    1   /* Login doesn't exist or password incorrect */
#define STATUS_INVALID    2   /* Password was incorrect */
#define STATUS_BLOCKED    3   /* UID is below minimum allowed to use this */
#define STATUS_EXPIRED    4   /* Login ID has passed it's expiration date */
#define STATUS_PW_EXPIRED 5   /* Password has expired and must be changed */
#define STATUS_NOLOGIN    6   /* Logins have been turned off */
#define STATUS_MANYFAILS  7   /* Bad login limit exceeded */

#define STATUS_INT_USER  50   /* pwauth was run by wrong uid */
#define STATUS_INT_ARGS  51   /* login/password not passed in correctly */
#define STATUS_INT_ERR   52   /* Miscellaneous internal errors */
#define STATUS_INT_NOROOT 53  /* pwauth cannot read password database */

#if defined(__cplusplus)
extern "C"
{
#endif
   
int check_auth(const char* login, const char* passwd);
   
#if defined(__cplusplus)
}
#endif

#endif //