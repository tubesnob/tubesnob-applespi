#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#include "../../orcadefaults.h"

#include "../../tslib/lib/tslib.h"
#include "../../tssocketlib/lib/tssocketlib_w5500.h"
#include "../../tssocketlib/lib/tssocketlib.h"

#ifndef __SPIDL_DOT_H__
#define __SPIDL_DOT_H__

typedef struct {
        unsigned long packetNumber;
        unsigned long totalNumberOfPackets;
        unsigned long packetDataLength;
        uint16_t packetCRC16;
} data_packet_t;

typedef struct
{
        char *source_macaddr;
        char *source_ipaddr;
        char *source_mask;
        char *source_gwaddr;
        int  source_port;
        char *dest_ip;
        int   dest_port;
        char *file_name;
        char *save_file_name;
        uint16_t packet_size;
} config_t;



#endif
