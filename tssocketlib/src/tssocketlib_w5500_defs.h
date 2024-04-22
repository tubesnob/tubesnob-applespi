
#ifndef __TSSOCKETLIBLIB_W5500_DEFS_DOT_C__
#define __TSSOCKETLIBLIB_W5500_DEFS_DOT_C__

#include "tssocketlib_w5500.h"

#define WORD unsigned short 
#define BYTE unsigned char 
#define DWORD unsigned int

#define W5500_RWB_READ  (0x00 << 2)
#define W5500_RWB_WRITE (0x01 << 2)

#define W5500_OMB_VDM  0x00
#define W5500_OMB_F1   0x01
#define W5500_OMB_F2   0x02
#define W5500_OMB_F4   0x03

#define W5500_ADDR_COMMON_MODE       0x0000
#define W5500_ADDR_COMMON_GWADDR0      0x0001
#define W5500_ADDR_COMMON_GWADDR1      0x0002
#define W5500_ADDR_COMMON_GWADDR2     0x0003
#define W5500_ADDR_COMMON_GWADDR3      0x0004
#define W5500_ADDR_COMMON_SUBMASK0     0x0005
#define W5500_ADDR_COMMON_SUBMASK1     0x0006
#define W5500_ADDR_COMMON_SUBMASK2     0x0007
#define W5500_ADDR_COMMON_SUBMASK3     0x0008
#define W5500_ADDR_COMMON_SRCMAC0     0x0009
#define W5500_ADDR_COMMON_SRCMAC1     0x000A
#define W5500_ADDR_COMMON_SRCMAC2     0x000B
#define W5500_ADDR_COMMON_SRCMAC3     0x000C
#define W5500_ADDR_COMMON_SRCMAC4     0x000D
#define W5500_ADDR_COMMON_SRCMAC5     0x000E
#define W5500_ADDR_COMMON_SRCIP0     0x000F
#define W5500_ADDR_COMMON_SRCIP1     0x0010
#define W5500_ADDR_COMMON_SRCIP2     0x0011
#define W5500_ADDR_COMMON_SRCIP3     0x0012
#define W5500_ADDR_COMMON_PHYCFG    0x002E
#define W5500_ADDR_COMMON_VERSIONR  0x0039

#define W5500_ADDR_SOCKET_MODE         0x0000
#define W5500_ADDR_SOCKET_COMMAND      0x0001
#define W5500_ADDR_SOCKET_INTERRUPT    0x0002
#define W5500_ADDR_SOCKET_STATUS       0x0003
#define W5500_ADDR_SOCKET_SRCPORT_HIGH 0x0004
#define W5500_ADDR_SOCKET_SRCPORT_LOW  0x0005
#define W5500_ADDR_SOCKET_DESTMAC0        0x0006
#define W5500_ADDR_SOCKET_DESTMAC1        0x0007
#define W5500_ADDR_SOCKET_DESTMAC2        0x0008
#define W5500_ADDR_SOCKET_DESTMAC3        0x0009
#define W5500_ADDR_SOCKET_DESTMAC4        0x000A
#define W5500_ADDR_SOCKET_DESTMAC5        0x000B
#define W5500_ADDR_SOCKET_DESTIP0         0x000C
#define W5500_ADDR_SOCKET_DESTIP1         0x000D
#define W5500_ADDR_SOCKET_DESTIP2         0x000E
#define W5500_ADDR_SOCKET_DESTIP3         0x000F
#define W5500_ADDR_SOCKET_DESTPORT_HIGH 0x0010
#define W5500_ADDR_SOCKET_DESTPORT_LOW  0x0011
#define W5500_ADDR_SOCKET_MAXSEG_HIGH  0x0012
#define W5500_ADDR_SOCKET_MAXSEG_LOW   0x0013
#define W5500_ADDR_SOCKET_TOS          0x0015
#define W5500_ADDR_SOCKET_TTL          0x0016
#define W5500_ADDR_SOCKET_RX_BUFSIZE    0x001E
#define W5500_ADDR_SOCKET_TX_BUFSIZE    0x001F
#define W5500_ADDR_SOCKET_TX_FREESIZE_HIGH    0x0020
#define W5500_ADDR_SOCKET_TX_FREESIZE_LOW   0x0021
#define W5500_ADDR_SOCKET_TX_READPTR_HIGH    0x0022
#define W5500_ADDR_SOCKET_TX_READPTR_LOW     0x0023
#define W5500_ADDR_SOCKET_TX_WRITEPTR_HIGH   0x0024
#define W5500_ADDR_SOCKET_TX_WRITEPTR_LOW    0x0025
#define W5500_ADDR_SOCKET_RX_RECVSIZE_HIGH    0x0026
#define W5500_ADDR_SOCKET_RX_RECVSIZE_LOW     0x0027
#define W5500_ADDR_SOCKET_RX_READPTR_HIGH    0x0028
#define W5500_ADDR_SOCKET_RX_READPTR_LOW     0x0029
#define W5500_ADDR_SOCKET_RX_WRITEPTR_HIGH   0x002A
#define W5500_ADDR_SOCKET_RX_WRITEPTR_LOW    0x002B
#define W5500_ADDR_SOCKET_INTMASK            0x002C
#define W5500_ADDR_SOCKET_FRAG_HIGH          0x002D
#define W5500_ADDR_SOCKET_FRAG_LOW           0x002E
#define W5500_ADDR_SOCKET_KEEPALIVE_TIME     0x002F

typedef struct {
        BYTE socket_register;
        BYTE socket_tx_buffer;
        BYTE socket_rx_buffer;
} w5500_bsb_socket_t;

typedef struct {
        BYTE common_register;
        w5500_bsb_socket_t sockets[8];
} w5500_bsb_t;


#define FT_W5500_GET_COMMON_8(__name__)     \
        int w5500_get_##__name__(BYTE* data) { \
                w5500_read_byte(W5500_ADDR_COMMON_##__name__, _bsb.common_register, data); \
                return W5500_OK; \
        }

#define FT_W5500_SET_COMMON_8(__name__)             \
        int w5500_set_COMMON_##__name__(WORD data) { \
                w5500_write_byte(W5500_ADDR_COMMON_##__name__, _bsb.common_register, data); \
                return W5500_OK; \
        }

#define FT_W5500_GET_COMMON_16(__name__)             \
        int w5500_get_##__name__(WORD* data) { \
                BYTE temp = 0; \
                w5500_read_byte(W5500_ADDR_COMMON_##__name__##_HIGH, _bsb.common_register, &temp); \
                *data = temp << 8; \
                w5500_read_byte(W5500_ADDR_COMMON_##__name__##_LOW, _bsb.common_register, &temp); \
                *data |= temp; \
                return W5500_OK; \
        }

#define FT_W5500_SET_COMMON_16(__name__)             \
        int w5500_set_##__name__(WORD data) { \
                w5500_write_byte(W5500_ADDR_COMMON_##__name__##_HIGH, _bsb.common_register, HIGH(data)); \
                w5500_write_byte(W5500_ADDR_COMMON_##__name__##_LOW, _bsb.common_register, LOW(data)); \
                return W5500_OK; \
        }

#define FT_W5500_GET_COMMON_BA4(__name__)             \
        int w5500_get_##__name__(BYTE* data) { \
                w5500_read_byte(W5500_ADDR_COMMON_##__name__##0, _bsb.common_register, &data[0]); \
                w5500_read_byte(W5500_ADDR_COMMON_##__name__##1, _bsb.common_register, &data[1]); \
                w5500_read_byte(W5500_ADDR_COMMON_##__name__##2, _bsb.common_register, &data[2]); \
                w5500_read_byte(W5500_ADDR_COMMON_##__name__##3, _bsb.common_register, &data[3]); \
                return W5500_OK; \
        }

#define FT_W5500_SET_COMMON_BA4(__name__)             \
        int w5500_set_##__name__(BYTE* data) { \
                w5500_write_byte(W5500_ADDR_COMMON_##__name__##0, _bsb.common_register, data[0]); \
                w5500_write_byte(W5500_ADDR_COMMON_##__name__##1, _bsb.common_register, data[1]); \
                w5500_write_byte(W5500_ADDR_COMMON_##__name__##2, _bsb.common_register, data[2]); \
                w5500_write_byte(W5500_ADDR_COMMON_##__name__##3, _bsb.common_register, data[3]); \
               return W5500_OK; \
        }

#define FT_W5500_GET_COMMON_BA6(__name__)             \
        int w5500_get_##__name__(BYTE* data) { \
                w5500_read_byte(W5500_ADDR_COMMON_##__name__##0, _bsb.common_register, &data[0]); \
                w5500_read_byte(W5500_ADDR_COMMON_##__name__##1, _bsb.common_register, &data[1]); \
                w5500_read_byte(W5500_ADDR_COMMON_##__name__##2, _bsb.common_register, &data[2]); \
                w5500_read_byte(W5500_ADDR_COMMON_##__name__##3, _bsb.common_register, &data[3]); \
                w5500_read_byte(W5500_ADDR_COMMON_##__name__##4, _bsb.common_register, &data[4]); \
                w5500_read_byte(W5500_ADDR_COMMON_##__name__##5, _bsb.common_register, &data[5]); \
                return W5500_OK; \
        }

#define FT_W5500_SET_COMMON_BA6(__name__)             \
        int w5500_set_##__name__(BYTE* data) { \
                w5500_write_byte(W5500_ADDR_COMMON_##__name__##0, _bsb.common_register, data[0]); \
                w5500_write_byte(W5500_ADDR_COMMON_##__name__##1, _bsb.common_register, data[1]); \
                w5500_write_byte(W5500_ADDR_COMMON_##__name__##2, _bsb.common_register, data[2]); \
                w5500_write_byte(W5500_ADDR_COMMON_##__name__##3, _bsb.common_register, data[3]); \
                w5500_write_byte(W5500_ADDR_COMMON_##__name__##4, _bsb.common_register, data[4]); \
                w5500_write_byte(W5500_ADDR_COMMON_##__name__##5, _bsb.common_register, data[5]); \
               return W5500_OK; \
        }

#define FT_W5500_GET_COMMON_TP8(__name__,__type__)             \
        int w5500_get_##__name__(__type__ *data) { \
        w5500_read_byte(W5500_ADDR_COMMON_##__name__, _bsb.common_register, FORCE_CAST(data, BYTE*)); \
                return W5500_OK; \
        }

#define FT_W5500_SET_COMMON_TP8(__name__,__type__)             \
        int w5500_set_##__name__(__type__ data) { \
        w5500_write_byte(W5500_ADDR_COMMON_##__name__, _bsb.common_register, FORCE_CAST(data, BYTE)); \
                return W5500_OK; \
        }

#define FT_W5500_GET_SOCKET_8(__name__)             \
        int w5500_get_socket_##__name__(BYTE socketNumber, BYTE* data) { \
                CHECK_SOCKET_NUMBER(socketNumber); \
                w5500_read_byte(W5500_ADDR_SOCKET_##__name__, _bsb.sockets[socketNumber].socket_register, data); \
                return W5500_OK; \
        }

#define FT_W5500_SET_SOCKET_8(__name__)             \
        int w5500_set_socket_##__name__(BYTE socketNumber, BYTE data) { \
                CHECK_SOCKET_NUMBER(socketNumber); \
                w5500_write_byte(W5500_ADDR_SOCKET_##__name__, _bsb.sockets[socketNumber].socket_register, data); \
                return W5500_OK; \
        }

#define FT_W5500_GET_SOCKET_16(__name__)             \
        int w5500_get_socket_##__name__(BYTE socketNumber, WORD* data) { \
                CHECK_SOCKET_NUMBER(socketNumber); \
                BYTE temp = 0; \
                w5500_read_byte(W5500_ADDR_SOCKET_##__name__##_HIGH, _bsb.sockets[socketNumber].socket_register, &temp); \
                *data = temp << 8; \
                w5500_read_byte(W5500_ADDR_SOCKET_##__name__##_LOW, _bsb.sockets[socketNumber].socket_register, &temp); \
                *data |= temp; \
                return W5500_OK; \
        }

#define FT_W5500_SET_SOCKET_16(__name__)             \
        int w5500_set_socket_##__name__(BYTE socketNumber, WORD data) { \
                CHECK_SOCKET_NUMBER(socketNumber); \
                w5500_write_byte(W5500_ADDR_SOCKET_##__name__##_HIGH, _bsb.sockets[socketNumber].socket_register, HIGH(data)); \
                w5500_write_byte(W5500_ADDR_SOCKET_##__name__##_LOW, _bsb.sockets[socketNumber].socket_register, LOW(data)); \
                return W5500_OK; \
        }

#define FT_W5500_GET_SOCKET_BA4(__name__)             \
        int w5500_get_socket_##__name__(BYTE socketNumber, BYTE* data) { \
                CHECK_SOCKET_NUMBER(socketNumber); \
                w5500_read_byte(W5500_ADDR_SOCKET_##__name__##0, _bsb.sockets[socketNumber].socket_register, &data[0]); \
                w5500_read_byte(W5500_ADDR_SOCKET_##__name__##1, _bsb.sockets[socketNumber].socket_register, &data[1]); \
                w5500_read_byte(W5500_ADDR_SOCKET_##__name__##2, _bsb.sockets[socketNumber].socket_register, &data[2]); \
                w5500_read_byte(W5500_ADDR_SOCKET_##__name__##3, _bsb.sockets[socketNumber].socket_register, &data[3]); \
                return W5500_OK; \
        }

#define FT_W5500_SET_SOCKET_BA4(__name__)             \
        int w5500_set_socket_##__name__(BYTE socketNumber, BYTE* data) { \
                CHECK_SOCKET_NUMBER(socketNumber); \
                w5500_write_byte(W5500_ADDR_SOCKET_##__name__##0, _bsb.sockets[socketNumber].socket_register, data[0]); \
                w5500_write_byte(W5500_ADDR_SOCKET_##__name__##1, _bsb.sockets[socketNumber].socket_register, data[1]); \
                w5500_write_byte(W5500_ADDR_SOCKET_##__name__##2, _bsb.sockets[socketNumber].socket_register, data[2]); \
                w5500_write_byte(W5500_ADDR_SOCKET_##__name__##3, _bsb.sockets[socketNumber].socket_register, data[3]); \
               return W5500_OK; \
        }

#define FT_W5500_GET_SOCKET_BA6(__name__)             \
        int w5500_get_socket_##__name__(BYTE socketNumber, BYTE* data) { \
                CHECK_SOCKET_NUMBER(socketNumber); \
                w5500_read_byte(W5500_ADDR_SOCKET_##__name__##0, _bsb.sockets[socketNumber].socket_register, &data[0]); \
                w5500_read_byte(W5500_ADDR_SOCKET_##__name__##1, _bsb.sockets[socketNumber].socket_register, &data[1]); \
                w5500_read_byte(W5500_ADDR_SOCKET_##__name__##2, _bsb.sockets[socketNumber].socket_register, &data[2]); \
                w5500_read_byte(W5500_ADDR_SOCKET_##__name__##3, _bsb.sockets[socketNumber].socket_register, &data[3]); \
                w5500_read_byte(W5500_ADDR_SOCKET_##__name__##4, _bsb.sockets[socketNumber].socket_register, &data[4]); \
                w5500_read_byte(W5500_ADDR_SOCKET_##__name__##5, _bsb.sockets[socketNumber].socket_register, &data[5]); \
                return W5500_OK; \
        }

#define FT_W5500_SET_SOCKET_BA6(__name__)             \
        int w5500_set_socket_##__name__(BYTE socketNumber, BYTE* data) { \
                CHECK_SOCKET_NUMBER(socketNumber); \
                w5500_write_byte(W5500_ADDR_SOCKET_##__name__##0, _bsb.sockets[socketNumber].socket_register, data[0]); \
                w5500_write_byte(W5500_ADDR_SOCKET_##__name__##1, _bsb.sockets[socketNumber].socket_register, data[1]); \
                w5500_write_byte(W5500_ADDR_SOCKET_##__name__##2, _bsb.sockets[socketNumber].socket_register, data[2]); \
                w5500_write_byte(W5500_ADDR_SOCKET_##__name__##3, _bsb.sockets[socketNumber].socket_register, data[3]); \
                w5500_write_byte(W5500_ADDR_SOCKET_##__name__##4, _bsb.sockets[socketNumber].socket_register, data[4]); \
                w5500_write_byte(W5500_ADDR_SOCKET_##__name__##5, _bsb.sockets[socketNumber].socket_register, data[5]); \
               return W5500_OK; \
        }

#endif