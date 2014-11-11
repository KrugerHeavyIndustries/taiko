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
#ifndef _TAIKO_SQLITE_H_
#define _TAIKO_SQLITE_H_

#include <sqlite3.h>
#include "exception.h"

using namespace std;

namespace taiko {
   
class sqlite3_t {
    public:
	sqlite3 *_D;

	sqlite3_t(const char *f)
	    : _D(0) {
		int r = sqlite3_open(f,&_D);
		if(r!=SQLITE_OK) {
		    string msg = sqlite3_errmsg(_D); sqlite3_close(_D);
		    throw taiko::exception(OPKELE_CP_ "Failed to open SQLite database: "+msg);
		}
	    }
	~sqlite3_t() {
	    if(_D) sqlite3_close(_D);
	}

	operator const sqlite3* () const { return _D; }
	operator sqlite3* () { return _D; }

	void exec(const char *sql) {
	    assert(_D);
	    char *errm;
	    if(sqlite3_exec(_D,sql,NULL,NULL,&errm)!=SQLITE_OK)
            throw opkele::exception(OPKELE_CP_ string("Failed to sqlite3_exec():")+errm);
	}
	void get_table(const char *sql,char ***resp,int *nr,int *nc) {
	    assert(_D);
	    char *errm;
	    if(sqlite3_get_table(_D,sql,resp,nr,nc,&errm)!=SQLITE_OK)
            throw taiko::exception(OPKELE_CP_ string("Failed to sqlite3_get_table():")+errm);
	}
};

template<typename T>
class sqlite3_mem_t {
    public:
	T _M;

	sqlite3_mem_t(T M) :_M(M) { }
	~sqlite3_mem_t() { if(_M) sqlite3_free(_M); }

	operator const T& () const { return _M; }
	operator T& () { return _M; }

	sqlite3_mem_t operator=(T M) {
	    if(_M) sqlite3_free(_M);
	    _M = M;
	}
};

class sqlite3_table_t {
    public:
	char **_T;

	sqlite3_table_t() : _T(0) { }
	sqlite3_table_t(char **T) : _T(T) { }
	~sqlite3_table_t() { if(_T) sqlite3_free_table(_T); }

	operator char**&(void) { return _T; }

	operator char ***(void) {
	    if(_T) sqlite3_free_table(_T);
	    return &_T; }

	const char *get(int r,int c,int nc) {
	    assert(_T);
	    return _T[r*nc+c];
	}
};
   
} // namespace taiko

#endif // if defined(_TAIKO_SQLITE_H_)
