#ifndef __TSLIB_DOT_H__
#define __TSLIB_DOT_H__

typedef unsigned short tslib_size_t;

// array type definition

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

// list type definition

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

#endif

