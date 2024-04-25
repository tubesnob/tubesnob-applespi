//#pragma noroot

#include "tslib.h"
#include "../../orcadefaults.h"

tsstring_vtbl_t*     _tsstring;

static void tsstring_resize(tsstring_t* self, tslib_size_t count) { }
static short tsstring_compare(tsstring_t* self, tsstring_t* other) { return 0; }
static void tsstring_insert(tsstring_t* self, char* insertString, tslib_size_t index) { }
static tsstring_t* tsstring_new(tslib_size_t initialSize);
static tsstring_t* tsstring_new_c(const char *initString);
static tsstring_t* tsstring_new_s(tsstring_t* initString);
static void tsstring_free(tsstring_t **self);
static tsstring_t*  tsstring_clone(tsstring_t* self);
static void tsstring_clear(tsstring_t* self); 
static tslib_size_t  tsstring_indexof(tsstring_t* self, const char *search); 
static tsstring_t*   tsstring_substring(tsstring_t* self, tslib_size_t start, tslib_size_t count); 
static tslist_t*  tsstring_split(tsstring_t* self, const char *splitter); 

tsstring_vtbl_t* tsstring_init() {
   if (_tsstring==NULL) {
      _tsstring = (tsstring_vtbl_t*) malloc(sizeof(tsstring_vtbl_t));
      _tsstring->new = &tsstring_new;
      _tsstring->new_c = &tsstring_new_c;
      _tsstring->new_s = &tsstring_new_s;
      _tsstring->free = &tsstring_free;
      _tsstring->clone = &tsstring_clone;
      _tsstring->clear = &tsstring_clear;
      _tsstring->compare = &tsstring_compare;
      _tsstring->resize = &tsstring_resize;
      _tsstring->split = &tsstring_split;
      _tsstring->substring = &tsstring_substring;
      _tsstring->indexof = &tsstring_indexof;
   }
   return _tsstring;
}

void tsstring_shutdown() {
    freeandnull(_tsstring);
}

static tsstring_t* tsstring_new(tslib_size_t initialSize) {
   tsstring_t* rv = (tsstring_t*) malloc(sizeof(tsstring_t));
   if (initialSize>0) {
      char *buffer = (char*) malloc(initialSize);
      if (buffer != NULL) {
         rv->buffersize = initialSize;
         rv->data = buffer;
      }
   }
   return rv;
}

static tsstring_t* tsstring_new_c(const char *initString) {
   if (initString==NULL) return NULL;
   tslib_size_t ilen = strlen(initString);
   tsstring_t* rv = _tsstring->new(ilen+1);
   _tsmem->copy(rv->data, initString, ilen);
   rv->data[ilen] = 0;
   return rv;
}

static tsstring_t* tsstring_new_s(tsstring_t* initString) {
   if (initString==NULL) return NULL;
   return _tsstring->new_c(initString->data);
}

static void tsstring_free(tsstring_t **self) {
   if (self==NULL) return;
   tsstring_t* p = *self;
   if (p!=NULL) {
      if (p->data != NULL) {
         free(p->data);
         p->data = NULL;
      }
      free(p);
   }
   self = NULL;
}

static tsstring_t* tsstring_clone(tsstring_t* self) {
   return _tsstring->new_s(self);
}

static void tsstring_clear(tsstring_t* self) {
   _tsmem->zero(self->data,self->buffersize);
}

static tslib_size_t tsstring_indexof(tsstring_t* self, const char *search) {
   char* ptr = strstr(self->data, search);
   if (ptr==NULL) return -1;
   return (tslib_size_t) (ptr - self->data);
}

static tsstring_t* tsstring_substring(tsstring_t* self, tslib_size_t start, tslib_size_t count) { 
   char* temp = (char*) malloc(count+1);
   _tsmem->zero(temp,count+1);
   _tsmem->copy(temp, (self->data)+start, count);
   tsstring_t* rv = _tsstring->new_c(temp);
   free(temp);
   return rv;
}

static tslist_t* tsstring_split(tsstring_t* self, const char *splitter) {

   short idx = 0;
   short splen = strlen(splitter);
   short slen = strlen((const char*) self->data);
   int spos = 0;
   char* sptr = (char*) self->data;
   char* found = NULL;

   tslist_t* rv = _tslist->new(8);

   do {
      found = strstr(sptr, splitter);
      idx = -1;
      if (found) {
         idx = found - sptr;
         tsstring_t* item = _tsstring->substring(self,spos,idx);
         _tslist->add(rv, item);
         spos += (idx + splen);
         sptr += (idx + splen);
      }
   } while(idx != -1);
   
   if (spos < slen) {
      tsstring_t* last = _tsstring->substring(self, spos, slen-spos);
      _tslist->add(rv, last);
   }
   return rv;
}



