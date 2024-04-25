//#pragma noroot

#include "tslib.h"
#include "../../orcadefaults.h"

tslist_vtbl_t* _tslist;

static void      tslist_add(tslist_t* self, void* item);
static void      tslist_insertAt(tslist_t* self, tslib_size_t index, void* item);
static void      tslist_removeAt(tslist_t* self, tslib_size_t index);
static void*     tslist_get(tslist_t* self, tslib_size_t index);
static void      tslist_set(tslist_t* self, tslib_size_t index, void* item);
static void      tslist_clear(tslist_t* self);
static void      tslist_free_callback_default(tslist_t* self, void* item);
static tslist_t* tslist_new(tslib_size_t chunkSize);

tslist_vtbl_t* tslist_init() {
    tslist_vtbl_t* rv = (tslist_vtbl_t*)malloc(sizeof(tslist_vtbl_t));
    rv->new = &tslist_new;
    rv->add = &tslist_add;
    rv->insertAt = &tslist_insertAt;
    rv->removeAt = &tslist_removeAt;
    rv->set = &tslist_set;
    rv->get = &tslist_get;
    rv->clear = &tslist_clear;
    rv->free_callback = &tslist_free_callback_default;
    _tslist = rv;
    return rv;
}

void tslist_shutdown() {
    freeandnull(_tslist);
}

static tslist_t* tslist_new(tslib_size_t chunkSize) {
   
   if (!chunkSize) chunkSize = 1;
   tslist_t* rv = NULL;
   rv = (tslist_t*) malloc(sizeof(tslist_t));
   if (!rv) return rv;
   rv->chunkSize = chunkSize;
   rv->data = _tsarray->new(rv->chunkSize);
   rv->count = 0;
   return rv;
}

static void tslist_check_resize(tslist_t* self, tslib_size_t checkSize) {
   if (checkSize > self->data->count) {
      tslib_size_t newSize = ((self->count / self->chunkSize)+1)*self->chunkSize;
      _tsarray->resize(self->data, newSize);
   }
   else {
      _tsarray->resize(self->data, checkSize);
   }
}

static void  tslist_add(tslist_t* self, void* item) {
   tslist_check_resize(self, self->count+1);
   _tsarray->set(self->data,self->count,item);
   self->count++;
}

static void  tslist_insertAt(tslist_t* self, tslib_size_t index, void* item) {
   if (index < 0 || index >= self->count) return;
   tslist_check_resize(self, self->count+1);
   tslib_size_t startIndex = index;
   tslib_size_t lastIndex = self->count;
   for(tslib_size_t moveIndex=lastIndex;moveIndex > startIndex; moveIndex--) {
        _tsarray->set(self->data,moveIndex,_tsarray->get(self->data,moveIndex-1));
   }
   _tsarray->set(self->data,index,item);
   self->count++;
}

static void  tslist_removeAt(tslist_t* self, tslib_size_t index) {
   tslib_size_t startIndex = index;
   tslib_size_t lastIndex = self->count-1;
   for(tslib_size_t moveIndex=startIndex;moveIndex < lastIndex;moveIndex++) {
    _tsarray->set(self->data,moveIndex,_tsarray->get(self->data,moveIndex+1));
   }
   tslist_check_resize(self, self->count-1);
   self->count--;
}
static void* tslist_get(tslist_t* self, tslib_size_t index) {
   if (index < 0 || index >= self->count) {
      return NULL;
   }
   return _tsarray->get(self->data,index);
}

static void  tslist_set(tslist_t* self, tslib_size_t index, void* item) {
   if (index < 0 || index >= self->count) {
      return;
   }
   _tsarray->set(self->data,index,item);
}

static void  tslist_clear(tslist_t* self) {
   _tsarray->clear(self->data);
   _tsarray->resize(self->data,self->chunkSize);
   self->count=0;
}

static void  tslist_free_callback_default(tslist_t* self, void* item) {
   // do nothing
}