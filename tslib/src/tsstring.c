#include "tslib.h"
#include "../../orcadefaults.h"

#pragma noroot

static tsstring_vtbl_t*   _tsstring;
static tsmem_vtbl_t*      _tsmem;

tsstring_t* tsstring_create(tslib_size_t initialSize) {
   tsstring_t* rv = (tsstring_t*) malloc(sizeof(tsstring_t));
   rv->f = _tsstring;
   if (initialSize>0) {
      char *buffer = (char*) malloc(initialSize);
      if (buffer != NULL) {
         rv->buffersize = initialSize;
         rv->data = buffer;
      }
   }
   return rv;
}

tsstring_t* tsstring_create_c(const char *initString) {
   if (initString==NULL) return NULL;
   tslib_size_t ilen = strlen(initString);
   tsstring_t* rv = _tsstring->create(ilen+1);
   _tsmem->copy(rv->data, initString, ilen);
   //printf("got a string @ %x\nval = %s\ntsmem @ %x\ncopy=%x\n",(int) rv, rv->data, (int) tsmem,(tsmem->copy));
   rv->data[ilen] = 0;
   return rv;
}

tsstring_t* tsstring_create_s(tsstring_t* initString) {
   if (initString==NULL) return NULL;
   return _tsstring->create_c(initString->data);
}

void tsstring_free(tsstring_t **self) {
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

tsstring_t*  tsstring_clone(tsstring_t* self) {
   return _tsstring->create_s(self);
}

void          tsstring_clear(tsstring_t* self) {
   _tsmem->zero(self->data,self->buffersize);
}

tslib_size_t  tsstring_indexof(tsstring_t* self, const char *search) {
   char* ptr = strstr(self->data, search);
   if (ptr==NULL) return -1;
   return (tslib_size_t) (ptr - self->data);
}

tsstring_t*   tsstring_substring(tsstring_t* self, tslib_size_t start, tslib_size_t count) { 
   char* temp = (char*) malloc(count+1);
   _tsmem->zero(temp,count+1);
   _tsmem->copy(temp, (self->data)+start, count);
   tsstring_t* rv = _tsstring->create_c(temp);
   free(temp);
   return rv;
}

tslist_t*  tsstring_split(tsstring_t* self, const char *splitter) {

   //printf("splitting source string [%s] based in splitter of [%s]\n", self->data, splitter);
   short idx = 0;
   short splen = strlen(splitter);
   short slen = strlen((const char*) self->data);
   int spos = 0;
   char* sptr = (char*) self->data;
   char* found = NULL;
   tslist_t* rv = tslist_create(8);
   do {
      printf("looking for splits\n");
      found = strstr(sptr, splitter);
      idx = -1;
      if (found) {
         idx = found - sptr;
         printf("found splitter at index %i\n", idx);
         tsstring_t* item = self->f->substring(self,spos,idx);
         printf("segment is [%s]\n", item->data);
         rv->add(rv, item);
         spos += (idx + splen);
         sptr += (idx + splen);
      }
   } while(idx != -1);
   
   if (spos < slen) {
      tsstring_t* last = self->f->substring(self, spos, slen-spos);
      rv->add(rv, last);
   }
   printf("returning %i segments\n",(int)rv->count);
   return rv;
}

void          tsstring_resize(tsstring_t* self, tslib_size_t count) { }
short         tsstring_compare(tsstring_t* self, tsstring_t* other) { return 0; }
void          tsstring_insert(tsstring_t* self, char* insertString, tslib_size_t index) { }



tsstring_vtbl_t* tsstring_init() {
   _tsmem = tsmem_init();
   if (_tsstring==NULL) {
      _tsstring = (tsstring_vtbl_t*) malloc(sizeof(tsstring_vtbl_t));
      _tsstring->create = &tsstring_create;
      _tsstring->create_c = &tsstring_create_c;
      _tsstring->create_s = &tsstring_create_s;
      _tsstring->free = &tsstring_free;
      _tsstring->clone = &tsstring_clone;
      _tsstring->clear = &tsstring_clear;
      _tsstring->compare = &tsstring_compare;
      _tsstring->resize = &tsstring_resize;
      _tsstring->split = &tsstring_split;
      _tsstring->substring = &tsstring_substring;
      _tsstring->indexof = &tsstring_indexof;
      //printf("tsstring=%x\n",(int) tsstring);
      //printf("tsstring.create=%x\n",(int)(tsstring->create));
      //printf("tsstring.create_c=%x\n",(int)(tsstring->create_c));
   }
   return _tsstring;
}


