#ifdef __APPLE2GS__
segment "AUTOSEG~~~";
#endif

#include "tslib.h"
#include "../../orcadefaults.h"

tsarray_vtbl_t* _tsarray;

static tsarray_t*   tsarray_new(tslib_size_t initialSize);
static void         tsarray_resize(tsarray_t* self, tslib_size_t newSize);
static void         tsarray_set(tsarray_t* self, tslib_size_t index, void* item);
static void*        tsarray_get(tsarray_t* self, tslib_size_t index);
static void         tsarray_clear(tsarray_t* self);
static void         tsarray_free_callback_default(tsarray_t* self, void* item);

tsarray_vtbl_t* tsarray_init() {
    tsarray_vtbl_t* rv = (tsarray_vtbl_t*) malloc(sizeof(tsarray_vtbl_t));
    rv->new = &tsarray_new;
    rv->resize = &tsarray_resize;
    rv->set = &tsarray_set;
    rv->get = &tsarray_get;
    rv->clear = &tsarray_clear;
    rv->free_callback = &tsarray_free_callback_default;
    _tsarray = rv;
    return rv;
}

void tsarray_shutdown() {
    freeandnull(_tsarray);
}

static tsarray_t* tsarray_new(tslib_size_t initialSize) {
   tsarray_t* rv = NULL;
   rv = (tsarray_t*) malloc(sizeof(tsarray_t));
   if (!rv) return rv;
   rv->data = NULL;
   rv->count = 0;
   _tsarray->resize(rv, initialSize);
   return rv;
}

static void tsarray_resize(tsarray_t* self, tslib_size_t newSize) {
   void** oldData = self->data;
   tslib_size_t oldSize = self->count;
   void** newData = (void**) malloc(sizeof(void*)*newSize);
   if (!newData) {
      return;
   }
   if (self->count != 0) {
      for(tslib_size_t index=0; index < oldSize; index++) {
         void* oldItem = oldData[index];
         if (index < newSize) {
            newData[index] = oldItem;
         }
         else {
            if (_tsarray->free_callback && oldItem) {
               _tsarray->free_callback(self,oldItem);
            }
         }
      }
      free(oldData);
   }
   self->data = newData;
   self->count = newSize;
}

static void  tsarray_set(tsarray_t* self, tslib_size_t index, void* item) {
   if (index > self->count || index < 0)
      return;
   self->data[index] = item;
}

static void* tsarray_get(tsarray_t* self, tslib_size_t index) {
   if (index > self->count || index < 0)
      return NULL;
   return self->data[index];
}

static void  tsarray_clear(tsarray_t* self) {
   for(tslib_size_t index=0; index < self->count; index++) {
      void* item = self->data[index];
        if (_tsarray->free_callback && item) {
            _tsarray->free_callback(self,item);
        }
      self->data[index] = NULL;
   }
}

static void  tsarray_free_callback_default(tsarray_t* self, void* item) {
   if (self && item) {
      // do nothing
   }
}
