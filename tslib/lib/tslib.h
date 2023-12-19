#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <memory.h>

#ifndef __TSLIB_DOT_H__
#define __TSLIB_DOT_H__

typedef unsigned short tslib_size_t;

//////////////////////////////////////
// TSMEM
//////////////////////////////////////
typedef struct tsmem_vtbl {
   void   (*copy)(const unsigned char *dest, const unsigned char *src, tslib_size_t count);
   void   (*zero)(const unsigned char *dest, tslib_size_t count);
   void   (*fill)(const unsigned char *dest, unsigned char val, tslib_size_t count);
   int    (*available)();
} tsmem_vtbl_t;
tsmem_vtbl_t* tsmem_init();

//////////////////////////////////////
// TSARRAY
//////////////////////////////////////

typedef struct tsarray_t {

   tslib_size_t   count;
   void** data;

   void   (*resize)(struct tsarray_t*, tslib_size_t);
   void   (*set)(struct tsarray_t*, tslib_size_t, void*);
   void*  (*get)(struct tsarray_t*, tslib_size_t);
   void   (*clear)(struct tsarray_t*);
   void   (*free_callback)(struct tsarray_t*, void*);
} tsarray_t;
tsarray_t* tsarray_create(tslib_size_t initialSize);

//////////////////////////////////////
// TSLIST
//////////////////////////////////////

typedef struct tslist_t {

   tslib_size_t       chunkSize;
   tslib_size_t       count;
   tsarray_t* data;
   
   void   (*add)(struct tslist_t*, void*);
   void*  (*get)(struct tslist_t*, tslib_size_t);
   void*  (*set)(struct tslist_t*, tslib_size_t, void*);
   void   (*insertAt)(struct tslist_t*, tslib_size_t, void*);
   void   (*removeAt)(struct tslist_t*, tslib_size_t);
   void   (*clear)(struct tslist_t*);
   void   (*free_callback)(struct tslist_t*, void*);
} tslist_t;
tslist_t* tslist_create();

//////////////////////////////////////
// TSSTRING
//////////////////////////////////////
// array type definition

struct tsstring_vtbl;
typedef struct tsstring_t {
   tslib_size_t    buffersize;    // size of the string buffer
   unsigned char*  data;          // pointer to the string buffer
   struct tsstring_vtbl* f;      // pointer to the function vtable
} tsstring_t;

// tsstring function pointers vtable
typedef struct tsstring_vtbl {
   tsstring_t*   (*create)(tslib_size_t initialSize);
   tsstring_t*   (*create_c)(const unsigned char *initString);
   tsstring_t*   (*create_s)(tsstring_t* initString);
   tsstring_t*   (*clone)(tsstring_t* self);
   void          (*clear)(tsstring_t* self);
   void          (*resize)(tsstring_t* self, tslib_size_t count);
   tslist_t*     (*split)(tsstring_t* self, const char *splitter);
   short         (*compare)(tsstring_t* self, tsstring_t* other);
   tsstring_t*   (*substring)(tsstring_t* self, tslib_size_t start, tslib_size_t count);
   tslib_size_t  (*indexof)(tsstring_t* self, char * search);
   void          (*free)(tsstring_t** self);
} tsstring_vtbl_t;

tsstring_vtbl_t*  tsstring_init();


//////////////////////////////////////
// UTILS
//////////////////////////////////////

void waitMilliseconds(int ms);
void waitSeconds(int seconds);

void DEBUG_LOG(const char *format, ...);

unsigned short gen_crc16(const unsigned char *data, unsigned short size);

char *strdup(const char *s);

#define FORCE_CAST(var, type) *(type*)&var
#define LOW(x)  (x & 0xFF)
#define HIGH(x) ((x >> 8) & 0xFF)

#endif

//////////////////////////////////////
// INI PARSER
//////////////////////////////////////

/* inih -- simple .INI file parser

inih is released under the New BSD license (see LICENSE.txt). Go to the project
home page for more info:

https://github.com/benhoyt/inih

*/

#ifndef __INI_H__
#define __INI_H__

/* Make this header file easier to include in C++ code */
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

/* Nonzero if ini_handler callback should accept lineno parameter. */
#ifndef INI_HANDLER_LINENO
#define INI_HANDLER_LINENO 0
#endif

/* Typedef for prototype of handler function. */
#if INI_HANDLER_LINENO
typedef int (*ini_handler)(void* user, const char* section,
                           const char* name, const char* value,
                           int lineno);
#else
typedef int (*ini_handler)(void* user, const char* section,
                           const char* name, const char* value);
#endif

/* Typedef for prototype of fgets-style reader function. */
typedef char* (*ini_reader)(char* str, int num, void* stream);

/* Parse given INI-style file. May have [section]s, name=value pairs
   (whitespace stripped), and comments starting with ';' (semicolon). Section
   is "" if name=value pair parsed before any section heading. name:value
   pairs are also supported as a concession to Python's configparser.

   For each name=value pair parsed, call handler function with given user
   pointer as well as section, name, and value (data only valid for duration
   of handler call). Handler should return nonzero on success, zero on error.

   Returns 0 on success, line number of first error on parse error (doesn't
   stop on first error), -1 on file open error, or -2 on memory allocation
   error (only when INI_USE_STACK is zero).
*/
int ini_parse(const char* filename, ini_handler handler, void* user);

/* Same as ini_parse(), but takes a FILE* instead of filename. This doesn't
   close the file when it's finished -- the caller must do that. */
int ini_parse_file(FILE* file, ini_handler handler, void* user);

/* Same as ini_parse(), but takes an ini_reader function pointer instead of
   filename. Used for implementing custom or string-based I/O (see also
   ini_parse_string). */
int ini_parse_stream(ini_reader reader, void* stream, ini_handler handler,
                     void* user);

/* Same as ini_parse(), but takes a zero-terminated string with the INI data
instead of a file. Useful for parsing INI data from a network socket or
already in memory. */
int ini_parse_string(const char* string, ini_handler handler, void* user);

/* Nonzero to allow multi-line value parsing, in the style of Python's
   configparser. If allowed, ini_parse() will call the handler with the same
   name for each subsequent line parsed. */
#ifndef INI_ALLOW_MULTILINE
#define INI_ALLOW_MULTILINE 1
#endif

/* Nonzero to allow a UTF-8 BOM sequence (0xEF 0xBB 0xBF) at the start of
   the file. See https://github.com/benhoyt/inih/issues/21 */
#ifndef INI_ALLOW_BOM
#define INI_ALLOW_BOM 1
#endif

/* Chars that begin a start-of-line comment. Per Python configparser, allow
   both ; and # comments at the start of a line by default. */
#ifndef INI_START_COMMENT_PREFIXES
#define INI_START_COMMENT_PREFIXES ";#"
#endif

/* Nonzero to allow inline comments (with valid inline comment characters
   specified by INI_INLINE_COMMENT_PREFIXES). Set to 0 to turn off and match
   Python 3.2+ configparser behaviour. */
#ifndef INI_ALLOW_INLINE_COMMENTS
#define INI_ALLOW_INLINE_COMMENTS 1
#endif
#ifndef INI_INLINE_COMMENT_PREFIXES
#define INI_INLINE_COMMENT_PREFIXES ";"
#endif

/* Nonzero to use stack for line buffer, zero to use heap (malloc/free). */
#ifndef INI_USE_STACK
#define INI_USE_STACK 1
#endif

/* Maximum line length for any line in INI file (stack or heap). Note that
   this must be 3 more than the longest line (due to '\r', '\n', and '\0'). */
#ifndef INI_MAX_LINE
#define INI_MAX_LINE 200
#endif

/* Nonzero to allow heap line buffer to grow via realloc(), zero for a
   fixed-size buffer of INI_MAX_LINE bytes. Only applies if INI_USE_STACK is
   zero. */
#ifndef INI_ALLOW_REALLOC
#define INI_ALLOW_REALLOC 0
#endif

/* Initial size in bytes for heap line buffer. Only applies if INI_USE_STACK
   is zero. */
#ifndef INI_INITIAL_ALLOC
#define INI_INITIAL_ALLOC 200
#endif

/* Stop parsing on first error (default is to keep parsing). */
#ifndef INI_STOP_ON_FIRST_ERROR
#define INI_STOP_ON_FIRST_ERROR 0
#endif

#ifdef __cplusplus
}
#endif

#endif /* __INI_H__ */


