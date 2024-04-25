#include "../../orcadefaults.h"
#include "../../tslib/src/tslib.h"
#include "../../tslib/src/getopt.h"
#include "../../tsspilib/src/tsspilib.h"
#include "../../tssocketlib/src/tssocketlib_w5500.h"
#include "../../tssocketlib/src/tssocketlib.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#ifndef __SPIDL_DOT_H__
#define __SPIDL_DOT_H__


#ifdef __APPLE2GS__
typedef struct {
        unsigned long packetNumber;
        unsigned long totalNumberOfPackets;
        unsigned long packetDataLength;
        uint16_t packetCRC16;
} data_packet_t;
#endif
#ifdef __MACOS__
#pragma pack(1)
typedef struct {
        uint32_t packetNumber;
        uint32_t totalNumberOfPackets;
        uint32_t packetDataLength;
        uint16_t packetCRC16;
} data_packet_t;
#endif

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
        char *config_file_name;
        uint16_t packet_size;
        uint8_t outputFileToSTDOUT;
} config_t;

int parse_command_line(int argc, char** argv, config_t *config);
int config_load(const char* configFileName, config_t* config);
config_t* config_cleanup(config_t* config);

#endif
