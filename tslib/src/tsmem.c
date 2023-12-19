#include "tslib.h"
#include "../../orcadefaults.h"
#pragma noroot

static tsmem_vtbl_t* _tsmem;

int tsmem_avail() { 
   return FreeMem();
}

void tsmem_copy(const unsigned char *dest, const unsigned char *src, tslib_size_t count) { 
   printf("copying %d bytes from %x to %x\n", (int) count, (int) src, (int) dest);
   unsigned char *dd = dest;
   unsigned char *ss = src;
   while(count-->0) {
      *dd++ = *src++;
   }
}

void tsmem_fill(const unsigned char *dest, unsigned char val, tslib_size_t count) { 
   unsigned char *dd = dest;
   while(count-->0)
      *dd++ = val;
}

void tsmem_zero(const unsigned char *dest, tslib_size_t count) { 
   tsmem_fill(dest, 0, count);
}

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

