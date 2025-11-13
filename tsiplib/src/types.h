#include <stdint.h>
#include "../../orcadefaults.h"

#ifndef TYPES_H
#define TYPES_H

/* Basic types for 16-bit 65816 processor */
/*typedef unsigned char  uint8_t;
typedef signed char    int8_t;
typedef unsigned int   uint16_t;
typedef signed int     int16_t;
typedef unsigned long  uint32_t;
typedef signed long    int32_t;
*/

/* Boolean type */
typedef unsigned char  bool;
#define true  1
#define false 0

/* NULL definition */
#ifndef NULL
#define NULL ((void*)0)
#endif


/* Network byte order conversions */
#define htons(x) ((uint16_t)(((x) >> 8) | ((x) << 8)))
#define ntohs(x) htons(x)
#define htonl(x) ((uint32_t)(((x) >> 24) | (((x) >> 8) & 0xFF00) | \
                            (((x) << 8) & 0xFF0000) | ((x) << 24)))
#define ntohl(x) htonl(x)

#endif /* TYPES_H */