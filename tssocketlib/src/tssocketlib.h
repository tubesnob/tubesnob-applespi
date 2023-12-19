#ifndef __TSSOCKETLIB_DOT_H__
#define __TSSOCKETLIB_DOT_H__

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
   unsigned char a0;
   unsigned char a1;
   unsigned char a2;
   unsigned char a3;
   unsigned char a4;
   unsigned char a5;
} address_t;


typedef struct socket_t {
   unsigned char  number;
   unsigned char  protocol;
   unsigned short id;
   address_t      dest_ip;
   unsigned short source_port;
   unsigned short dest_port;
   unsigned char  status;
   unsigned short rx_bytes_available;
   int (*close)(struct socket_t *);
   int (*connect)(struct socket_t *);
   int (*disconnect)(struct socket_t *);
   int (*listen)(struct socket_t *);
   int (*send)(struct socket_t *, unsigned char *, unsigned short);
   int (*receive_available)(struct socket_t *, unsigned short *);
   int (*receive)(struct socket_t *, unsigned char *, unsigned short);
   int (*refresh)(struct socket_t *);
} socket_t;

void socket_init();

int socket_create(unsigned char protocol, unsigned short source_port, socket_t** socket);
char* iptostr(address_t ip, char* buf);
int strtoip(const char *str, address_t* ip);
char* mactostr(address_t mac, char *buf);
int strtomac(const char *str, address_t* mac);
address_t* addrcpy(address_t* dest, const address_t* source);

void dump_socketStatus(socket_t* socket);

#endif