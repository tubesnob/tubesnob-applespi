#include "../../orcadefaults.h"

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
        uint8_t outputFileToSTDOUT;
} config_t;



#endif
