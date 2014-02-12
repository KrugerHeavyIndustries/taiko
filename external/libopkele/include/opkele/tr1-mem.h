#ifndef __OPKELE_TR1_MEM_H
#define __OPKELE_TR1_MEM_H

#if defined(__APPLE__)
#include <memory>
#else
#include <tr1/memory>
#endif

namespace opkele {
#if defined(__APPLE__)
   namespace ptr = std;
#else
   namespace ptr = std::tr1;
#endif
}

#endif /* __OPKELE_TR1_MEM_H */
