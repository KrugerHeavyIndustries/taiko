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
#include "mongoose_connection.h"

namespace taiko {
   
template<typename IT>
class join_iterator : public iterator<
		      input_iterator_tag,typename IT::value_type,
		      void,typename IT::pointer,typename IT::reference> {
    public:
	typedef pair<IT,IT> range_t;
	typedef list<range_t> ranges_t;
	ranges_t ranges;

	join_iterator() { }

	bool cleanup() {
	    bool rv = false;
	    while(!(ranges.empty() || ranges.front().first!=ranges.front().second)) {
		ranges.pop_front(); rv = true;
	    }
	    return rv;
	}

	join_iterator<IT>& add_range(const IT& b,const IT& e) {
	    ranges.push_back(typename ranges_t::value_type(b,e));
	    cleanup();
	    return *this;
	}

	bool operator==(const join_iterator<IT>& x) const {
	    return ranges==x.ranges; }
	bool operator!=(const join_iterator<IT>& x) const {
	    return ranges!=x.ranges; }

	typename IT::reference operator*() const {
	    assert(!ranges.empty());
	    assert(ranges.front().first!=ranges.front().second);
	    return *ranges.front().first; }
	typename IT::pointer operator->() const {
	    assert(!ranges.empty());
	    assert(ranges.front().first!=ranges.front().second);
	    return ranges.front().first.operator->(); }

	join_iterator<IT>& operator++() {
	    cleanup();
	    if(ranges.empty()) return *this;
	    do {
		++ranges.front().first;
	    }while(cleanup() && !ranges.empty());
	    return *this;
	}
	join_iterator<IT> operator++(int) {
	    join_iterator<IT> rv(*this);
	    ++(*this); return rv; }
};

template<typename IT>
class cut_prefix_filterator : public opkele::util::basic_filterator<IT> {
    public:
	string pfx;
	mutable string tmp;

	cut_prefix_filterator() { }
	cut_prefix_filterator(const IT& _bi,const IT&_ei,const string& p)
        : opkele::util::basic_filterator<IT>(_bi,_ei), pfx(p) {
            this->prepare();
    }

	bool is_interesting() const {
	    return pfx.length()==0 || !strncmp(this->it->c_str(),pfx.c_str(),pfx.length());
	}

	typename IT::reference operator*() const {
	    assert(!this->empty);
	    tmp = *this->it; tmp.erase(0,pfx.length());
	    return tmp; }
	typename IT::pointer operator->() const {
	    assert(!this->empty);
	    return &this->operator*(); }
};

template<typename IT>
class null_filter : public opkele::util::basic_filterator<IT> {
   public:
   
      null_filter() {}
      null_filter(const IT& _bi, const IT& _ei)
         : opkele::util::basic_filterator<IT>(_bi, _ei)
      {
      }
   
      bool is_interesting() const {
         return true;
      }
   
//      typename IT::reference operator*() const {
//         assert(!this->empty);
//         return *this->it;
//      }
//   
//      typename IT::pointer operator->() const {
//         assert(!this->empty);
//         return &this->operator*();
//      }
};

class kingate_openid_message_t : public opkele::basic_openid_message {
   typedef join_iterator<opkele::params_t::const_iterator> jitterator;
	typedef opkele::util::map_keys_iterator<
	    jitterator,
	    fields_iterator::value_type,
	    fields_iterator::reference,
	    fields_iterator::pointer> keys_iterator;

	typedef cut_prefix_filterator<keys_iterator> pfilterator;
   
   //typedef null_filter<keys_iterator> nullfilter;
   
   public:
   
   const mongoose_connection_t& gw;

	kingate_openid_message_t(const mongoose_connection_t& g) : gw(g) { }
   virtual ~kingate_openid_message_t() {}

	virtual bool has_field(const string& n) const {
	    return gw.has_param("openid." + n);
    }
    
	virtual const string& get_field(const string& n) const {
      try {
         return gw.get_param("openid." + n);
      } catch(taiko::exception_notfound& nf) {
         throw opkele::failed_lookup(OPKELE_CP_ nf.what());
      }
   }

	fields_iterator fields_begin() const {
	    return pfilterator(
                 keys_iterator(
                    jitterator()
                    .add_range( gw.get.begin(), gw.get.end() )
                    .add_range( gw.post.begin(), gw.post.end() ),
                     jitterator()),
                         keys_iterator(), "openid.");
	}
   
	fields_iterator fields_end() const {
	    return pfilterator();
	}
};
   
} //  namespace taiko
