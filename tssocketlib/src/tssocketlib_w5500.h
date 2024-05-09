#include "../../orcadefaults.h"

#ifndef __TSSOCKETLIB_W5500_DOT_H__
#define __TSSOCKETLIB_W5500_DOT_H__

#define W5500_MAX_SOCKETS       0x08

#define W5500_OK                      0x0000
#define W5500_ERR_NOHARDWARE          0x1000
#define W5500_ERR_NOTINITIALIZED      0x2000
#define W5500_ERR_INVALIDSOCKETNUMBER 0x3000
#define W5500_ERR_NOSOCKETSAVAILABLE  0x3001

#define W5500_PHYCFG_OPMODE_10BT_HALF_NOAUTO   0x00
#define W5500_PHYCFG_OPMODE_10BT_FULL_NOAUTO   0x01
#define W5500_PHYCFG_OPMODE_100BT_HALF_NOAUTO  0x02
#define W5500_PHYCFG_OPMODE_100BT_FULL_NOAUTO  0x03
#define W5500_PHYCFG_OPMODE_100BT_HALF_AUTO    0x04
#define W5500_PHYCFG_OPMODE_NOTUSED            0x05
#define W5500_PHYCFG_OPMODE_POWERDOWN          0x06
#define W5500_PHYCFG_OPMODE_ALL_CAPABLE_AUTO   0x07
#define W5500_PHYCFG_DUPLEX_HALF 0x00
#define W5500_PHYCFG_DUPLEX_FULL 0x01
#define W5500_PHYCFG_SPEED_10MBPS  0x00
#define W5500_PHYCFG_SPEED_100MBPS 0x01
#define W5500_PHYCFG_LINKSTATUS_DOWN 0x00
#define W5500_PHYCFG_LINKSTATUS_UP   0x01

#define W5500_SOCKET_MODE_CLOSE    0x00
#define W5500_SOCKET_MODE_TCP    0x21
#define W5500_SOCKET_MODE_UDP    0x02
#define W5500_SOCKET_MODE_IPRAW  0x03
#define W5500_SOCKET_MODE_MACRAW 0x04
#define W5500_SOCKET_MODE_PPPOE  0x05
#define W5500_SOCKET_MODE_ND     0x20
#define W5500_SOCKET_MODE_MULTI  0x80

#define W5500_SOCKET_CMD_OPEN       0x01
#define W5500_SOCKET_CMD_LISTEN     0x02
#define W5500_SOCKET_CMD_CONNECT    0x04
#define W5500_SOCKET_CMD_DISCON     0x08
#define W5500_SOCKET_CMD_CLOSE      0x10
#define W5500_SOCKET_CMD_SEND       0x20
#define W5500_SOCKET_CMD_SEND_MAC   0x21
#define W5500_SOCKET_CMD_SEND_KEEP  0x22
#define W5500_SOCKET_CMD_RECV       0x40

#define W5500_SOCKET_INTERRUPT_SENDOK         0x10
#define W5500_SOCKET_INTERRUPT_TIMEOUT        0x08
#define W5500_SOCKET_INTERRUPT_RECV           0x04
#define W5500_SOCKET_INTERRUPT_DISCONNECTED   0x02
#define W5500_SOCKET_INTERRUPT_CONNECTED      0x01

#define W5500_SOCKET_STATUS_CLOSED      0x00
#define W5500_SOCKET_STATUS_INIT        0x13
#define W5500_SOCKET_STATUS_LISTEN      0x14
#define W5500_SOCKET_STATUS_SYNSENT     0x15
#define W5500_SOCKET_STATUS_SYNRECV     0x16
#define W5500_SOCKET_STATUS_ESTABLISHED 0x17
#define W5500_SOCKET_STATUS_FIN_WAIT    0x18
#define W5500_SOCKET_STATUS_CLOSING     0x1A
#define W5500_SOCKET_STATUS_TIME_WAIT   0x1B
#define W5500_SOCKET_STATUS_CLOSE_WAIT  0x1C
#define W5500_SOCKET_STATUS_LAST_ACK    0x1D
#define W5500_SOCKET_STATUS_UDP         0x22
#define W5500_SOCKET_STATUS_IPRAW       0x32
#define W5500_SOCKET_STATUS_MACRAW      0x42
#define W5500_SOCKET_STATUS_PPPOE       0x5F

#define W5500_SOCKET_IPPROTO_IP    0x00
#define W5500_SOCKET_IPPROTO_ICMP  0x01
#define W5500_SOCKET_IPPROTO_IGMP  0x02
#define W5500_SOCKET_IPPROTO_GGP   0x03
#define W5500_SOCKET_IPPROTO_TCP   0x06
#define W5500_SOCKET_IPPROTO_PUP   0x0C
#define W5500_SOCKET_IPPROTO_UDP   0x11
#define W5500_SOCKET_IPPROTO_IDP   0x16
#define W5500_SOCKET_IPPROTO_ND    0x4D
#define W5500_SOCKET_IPPROTO_RAW   0xFF

typedef struct {
   unsigned char  reset : 1;
   unsigned char  opmode_set : 1;
   unsigned char  opmode : 3;
   unsigned char  duplex_status : 1;
   unsigned char  speed_status : 1;
   unsigned char  link_status : 1;
} w5500_phycfg_t;

int w5500_init();
int w5500_close();
int w5500_reset();
int w5500_dump_state();
int w5500_socket_write_data(unsigned char  socketNumber,  unsigned char  *buf, unsigned short  size);
int w5500_socket_read_data(unsigned char  socketNumber,  unsigned char  *buf, unsigned short  size);
int w5500_set_phyopmode(uint8_t phyopmode);

#define HT_W5500_GET_COMMON_8(__name__)    int w5500_get_##__name__(unsigned char * data); 
#define HT_W5500_SET_COMMON_8(__name__)    int w5500_set_##__name__(unsigned short data); 
#define HT_W5500_GET_COMMON_16(__name__)   int w5500_get_##__name__(unsigned short* data); 
#define HT_W5500_SET_COMMON_16(__name__)   int w5500_set_##__name__(unsigned short data);
#define HT_W5500_GET_COMMON_BA4(__name__)  int w5500_get_##__name__(unsigned char * data); 
#define HT_W5500_SET_COMMON_BA4(__name__)  int w5500_set_##__name__(unsigned char * data); 
#define HT_W5500_GET_COMMON_BA6(__name__)  int w5500_get_##__name__(unsigned char * data); 
#define HT_W5500_SET_COMMON_BA6(__name__)  int w5500_set_##__name__(unsigned char * data);
#define HT_W5500_GET_COMMON_TP8(__name__,__type__)  int w5500_get_##__name__(__type__ *data); 
#define HT_W5500_SET_COMMON_TP8(__name__,__type__)  int w5500_set_##__name__(__type__ data);

HT_W5500_GET_COMMON_8(VERSIONR)
HT_W5500_GET_COMMON_BA4(GWADDR)
HT_W5500_SET_COMMON_BA4(GWADDR)
HT_W5500_GET_COMMON_BA4(SRCIP)
HT_W5500_SET_COMMON_BA4(SRCIP)
HT_W5500_GET_COMMON_BA4(SUBMASK)
HT_W5500_SET_COMMON_BA4(SUBMASK)
HT_W5500_GET_COMMON_BA6(SRCMAC)
HT_W5500_SET_COMMON_BA6(SRCMAC)
HT_W5500_GET_COMMON_TP8(PHYCFG, w5500_phycfg_t)
HT_W5500_SET_COMMON_TP8(PHYCFG, w5500_phycfg_t)

#define HT_W5500_GET_SOCKET_8(__name__)    int w5500_get_socket_##__name__(unsigned char  socketNumber, unsigned char * data); 
#define HT_W5500_SET_SOCKET_8(__name__)    int w5500_set_socket_##__name__(unsigned char  socketNumber, unsigned char  data); 
#define HT_W5500_GET_SOCKET_16(__name__)   int w5500_get_socket_##__name__(unsigned char  socketNumber, unsigned short* data); 
#define HT_W5500_SET_SOCKET_16(__name__)   int w5500_set_socket_##__name__(unsigned char  socketNumber, unsigned short  data);
#define HT_W5500_GET_SOCKET_BA4(__name__)  int w5500_get_socket_##__name__(unsigned char  socketNumber, unsigned char * data); 
#define HT_W5500_SET_SOCKET_BA4(__name__)  int w5500_set_socket_##__name__(unsigned char  socketNumber, unsigned char * data); 
#define HT_W5500_GET_SOCKET_BA6(__name__)  int w5500_get_socket_##__name__(unsigned char  socketNumber, unsigned char * data); 
#define HT_W5500_SET_SOCKET_BA6(__name__)  int w5500_set_socket_##__name__(unsigned char  socketNumber, unsigned char * data);

HT_W5500_SET_SOCKET_8(COMMAND)
HT_W5500_GET_SOCKET_8(STATUS)
HT_W5500_SET_SOCKET_8(STATUS)
HT_W5500_GET_SOCKET_8(MODE)
HT_W5500_SET_SOCKET_8(MODE)
HT_W5500_GET_SOCKET_16(SRCPORT)
HT_W5500_SET_SOCKET_16(SRCPORT)
HT_W5500_GET_SOCKET_16(DESTPORT)
HT_W5500_SET_SOCKET_16(DESTPORT)
HT_W5500_GET_SOCKET_8(INTERRUPT)
HT_W5500_SET_SOCKET_8(INTERRUPT)
HT_W5500_GET_SOCKET_BA4(DESTIP)
HT_W5500_SET_SOCKET_BA4(DESTIP)
HT_W5500_GET_SOCKET_BA6(DESTMAC)
HT_W5500_SET_SOCKET_BA6(DESTMAC)
HT_W5500_GET_SOCKET_16(MAXSEG)
HT_W5500_SET_SOCKET_16(MAXSEG)
HT_W5500_GET_SOCKET_8(TOS)
HT_W5500_SET_SOCKET_8(TOS)
HT_W5500_GET_SOCKET_8(TTL)
HT_W5500_SET_SOCKET_8(TTL)
HT_W5500_GET_SOCKET_8(INTMASK)
HT_W5500_SET_SOCKET_8(INTMASK)
HT_W5500_GET_SOCKET_16(FRAG)
HT_W5500_SET_SOCKET_16(FRAG)
HT_W5500_GET_SOCKET_8(KEEPALIVE_TIME)
HT_W5500_SET_SOCKET_8(KEEPALIVE_TIME)
HT_W5500_GET_SOCKET_8(RX_BUFSIZE)
HT_W5500_SET_SOCKET_8(RX_BUFSIZE)
HT_W5500_GET_SOCKET_16(RX_RECVSIZE)
HT_W5500_GET_SOCKET_16(RX_READPTR)
HT_W5500_SET_SOCKET_16(RX_READPTR)
HT_W5500_GET_SOCKET_16(RX_WRITEPTR)
HT_W5500_GET_SOCKET_8(TX_BUFSIZE)
HT_W5500_SET_SOCKET_8(TX_BUFSIZE)
HT_W5500_GET_SOCKET_16(TX_FREESIZE)
HT_W5500_GET_SOCKET_16(TX_WRITEPTR)
HT_W5500_SET_SOCKET_16(TX_WRITEPTR)



#endif
