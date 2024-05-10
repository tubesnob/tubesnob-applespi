#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <stdint.h>

#include "../../tslib/src/tslib.h"
#include "../../tsspilib/src/tsspilib.h"
#include "../../tssocketlib/src/tssocketlib.h"
#include "../../tssocketlib/src/tssocketlib_w5500.h"

#ifndef __SPIWS_DOT_H__
#define __SPIWS_DOT_H__

typedef struct {
        char *source_macaddr;
        char *source_ipaddr;
        char *source_mask;
        char *source_gwaddr;
        int   source_listenport;
        char *root_folder;
 } config_t;


typedef struct {
   uint8_t* method;
   uint8_t* uri;
   uint8_t* protocol;
} http_request;

int config_handler(void* user, const char* section, const char* name, const char* value);
int spiws_init_w5500(config_t* config);
int spiws_server_run(config_t* config);

#endif