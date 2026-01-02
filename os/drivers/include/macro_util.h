#ifndef __DD_COMMON_H__
#define __DD_COMMON_H__

#include <stddef.h>

#define Z_STRINGIFY(x) #x
#define STRINGIFY(s) Z_STRINGIFY(s)

/* concatenate the values of the arguments into one */
#define _DO_CONCAT(x, y) x ## y
#define _CONCAT(x, y) _DO_CONCAT(x, y)

#define OFFSETOF(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#define CONTAINER_OF(ptr, type, member) ({                      \
      const typeof( ((type *)0)->member ) *__mptr = (ptr);      \
      (type *)((char *)__mptr - OFFSETOF(type,member));})

#endif/*__DD_COMMON_H__*/

