#include "../../orcadefaults.h"

#ifndef __TSIPLIB_W5500_RAW_SOCKET_DOT_H__
#define __TSIPLIB_W5500_RAW_SOCKET_DOT_H__

#define SOCKET_PROTOCOL_IP    0x00
#define SOCKET_PROTOCOL_ICMP  0x01
#define SOCKET_PROTOCOL_IGMP  0x02
#define SOCKET_PROTOCOL_TCP   0x06
#define SOCKET_PROTOCOL_UDP   0x11
#define SOCKET_PROTOCOL_RAW   0xFF

#define SOCKET_OK                   0x0000
#define SOCKET_ERR_INVALIDARGUMENT  0x1000
#define SOCKET_NORESOURCES          0x3000

#define SOCKET_STATUS_CLOSED      0x00
#define SOCKET_STATUS_INIT        0x13
#define SOCKET_STATUS_LISTEN      0x14
#define SOCKET_STATUS_ESTABLISHED 0x17
#define SOCKET_STATUS_UDP         0x22

#define MAX_SOCKETS 0x08

typedef struct {
   uint8_t a0;
   uint8_t a1;
   uint8_t a2;
   uint8_t a3;
   uint8_t a4;
   uint8_t a5;
} address_t;

typedef struct w5500_raw_socket_t{
   uint8_t      number;
   uint8_t      protocol;
   uint16_t     id;
   uint8_t      status;
   uint16_t     rx_bytes_available;
   int (*close)(struct w5500_raw_socket_t *);
   int (*send)(struct w5500_raw_socket_t *, uint8_t* , uint16_t);
   int (*receive_available)(struct w5500_raw_socket_t *, uint16_t*);
   int (*receive)(struct w5500_raw_socket_t *, uint8_t* , uint16_t);
   int (*refresh)(struct w5500_raw_socket_t *);
} w5500_raw_socket_t;

void w5500_raw_socket_init();
w5500_raw_socket_t* w5500_raw_socket_create();
void w5500_raw_socket_dump_socketStatus(w5500_raw_socket_t* socket);

#endif