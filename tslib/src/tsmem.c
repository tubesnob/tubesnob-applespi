#pragma noroot

#include "tslib.h"
#include "../../orcadefaults.h"

tsmem_vtbl_t* _tsmem;

static int tsmem_avail();
static void tsmem_copy(const char *dest, const char *src, tslib_size_t count);
static void tsmem_fill(const char *dest, char val, tslib_size_t count);
static void tsmem_zero(const char *dest, tslib_size_t count);

tsmem_vtbl_t* tsmem_init() {
   if (_tsmem==NULL) {
      _tsmem = (tsmem_vtbl_t*) malloc(sizeof(tsmem_vtbl_t));
      _tsmem->copy = &tsmem_copy;
      _tsmem->fill = &tsmem_fill;
      _tsmem->zero = &tsmem_zero;
      _tsmem->available = &tsmem_avail;
   }
   return _tsmem;
}

void tsmem_shutdown() {
    freeandnull(_tsmem);
}

static int tsmem_avail() {
   #ifdef __MACOS__
   return 1;
   #else 
   return FreeMem();
   #endif
}

static void tsmem_copy(const char *dest, const char *src, tslib_size_t count) { 
   char *dd = (char*) dest;
   char *ss = (char*) src;
   while(count-->0) {
      *dd++ = *src++;
   }
}

static void tsmem_fill(const char *dest, char val, tslib_size_t count) { 
   char *dd = (char*)dest;
   while(count-->0)
      *dd++ = val;
}

static void tsmem_zero(const char *dest, tslib_size_t count) { 
   tsmem_fill(dest, 0, count);
}
