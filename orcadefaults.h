#ifndef __ORCADEFAULTS_DOT_H__
#define __ORCADEFAULTS_DOT_H__

#pragma optimize -1
#pragma ignore 0x0018

#ifndef NULL
#define NULL 0
#endif

#define null
static int __useTimeTool = 0;

typedef unsigned int    uint32_t;
typedef unsigned short  uint16_t;
typedef unsigned char   uint8_t;
typedef signed int      int32_t; 
typedef signed short    int16_t;
typedef signed char     int8_t;


#define freeandnull(__ptr__)    { if (__ptr__ != NULL) { free(__ptr__); __ptr__=NULL; } }


#endif