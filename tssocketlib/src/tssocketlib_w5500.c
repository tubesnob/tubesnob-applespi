#include "tssocketlib_w5500.h"
#include "tssocketlib_w5500_defs.h"
#include "../../tslib/lib/tslib.h"
#include "../../tsspilib/lib/tsspilib.h"

#include "../../orcadefaults.h"
#pragma noroot

w5500_bsb_t _bsb;

BYTE _spi_header_buffer[3];

#define CHECK_SOCKET_NUMBER(__x_socket_number__) { if (__x_socket_number__ >= W5500_MAX_SOCKETS) return W5500_ERR_INVALIDSOCKETNUMBER; }

int w5500_write_byte(int address, BYTE bsb, BYTE data);
int w5500_read_byte(int address, BYTE bsb, BYTE *data);
int w5500_write_mult(int address, BYTE bsb, BYTE* data, WORD size);
int w5500_read_mult(int address, BYTE bsb, BYTE* data, WORD size);

int w5500_init() {

   _bsb.common_register = 0x00;
   for(int socketNumber=0; socketNumber < W5500_MAX_SOCKETS; socketNumber++) {
      _bsb.sockets[socketNumber].socket_register  = ((socketNumber << 2) | 0x01) << 3;
      _bsb.sockets[socketNumber].socket_tx_buffer = ((socketNumber << 2) | 0x02) << 3;
      _bsb.sockets[socketNumber].socket_rx_buffer = ((socketNumber << 2) | 0x03) << 3;
   }

   w5500_reset();

   for(int socketNumber=0; socketNumber < W5500_MAX_SOCKETS; socketNumber++) {
        w5500_set_socket_INTERRUPT(socketNumber, 0xFF);         // reset the interrupt register
        w5500_set_socket_INTMASK(socketNumber, 0xFF);           // set the interrupt mask register to trigger all interrupts
        w5500_set_socket_TX_BUFSIZE(socketNumber, 0x02);        // set the tx buffer to 2k
        w5500_set_socket_RX_BUFSIZE(socketNumber, 0x02);        // set the rx buffer to 2k
        w5500_set_socket_COMMAND(socketNumber, W5500_SOCKET_CMD_CLOSE); // make sure all sockets are closed
   }

   return W5500_OK;
}

int w5500_close() {
   spi_end_trans();
   return W5500_OK;
}

int w5500_reset() {
        BYTE mr = 0x80;
        w5500_write_byte(W5500_ADDR_COMMON_MODE, _bsb.common_register, mr);

        // wait for the device to actually reset....
        while(mr & 0x80) {
                w5500_read_byte(W5500_ADDR_COMMON_MODE, _bsb.common_register, &mr);
        }        
        return W5500_OK;
}


FT_W5500_GET_COMMON_8(VERSIONR)
FT_W5500_GET_COMMON_BA4(GWADDR)
FT_W5500_SET_COMMON_BA4(GWADDR)
FT_W5500_GET_COMMON_BA4(SRCIP)
FT_W5500_SET_COMMON_BA4(SRCIP)
FT_W5500_GET_COMMON_BA4(SUBMASK)
FT_W5500_SET_COMMON_BA4(SUBMASK)
FT_W5500_GET_COMMON_BA6(SRCMAC)
FT_W5500_SET_COMMON_BA6(SRCMAC)
FT_W5500_GET_COMMON_TP8(PHYCFG, w5500_phycfg_t)
FT_W5500_SET_COMMON_TP8(PHYCFG, w5500_phycfg_t)


FT_W5500_SET_SOCKET_8(COMMAND)
FT_W5500_GET_SOCKET_8(STATUS)
FT_W5500_SET_SOCKET_8(STATUS)
FT_W5500_GET_SOCKET_8(MODE)
FT_W5500_SET_SOCKET_8(MODE)
FT_W5500_GET_SOCKET_8(INTERRUPT)
FT_W5500_SET_SOCKET_8(INTERRUPT)
FT_W5500_GET_SOCKET_8(INTMASK)
FT_W5500_SET_SOCKET_8(INTMASK)
FT_W5500_GET_SOCKET_16(SRCPORT)
FT_W5500_SET_SOCKET_16(SRCPORT)
FT_W5500_GET_SOCKET_16(DESTPORT)
FT_W5500_SET_SOCKET_16(DESTPORT)
FT_W5500_GET_SOCKET_BA4(DESTIP)
FT_W5500_SET_SOCKET_BA4(DESTIP)
FT_W5500_GET_SOCKET_BA6(DESTMAC)
FT_W5500_SET_SOCKET_BA6(DESTMAC)
FT_W5500_GET_SOCKET_16(MAXSEG)
FT_W5500_SET_SOCKET_16(MAXSEG)
FT_W5500_GET_SOCKET_8(TOS)
FT_W5500_SET_SOCKET_8(TOS)
FT_W5500_GET_SOCKET_8(TTL)
FT_W5500_SET_SOCKET_8(TTL)
FT_W5500_GET_SOCKET_16(FRAG)
FT_W5500_SET_SOCKET_16(FRAG)
FT_W5500_GET_SOCKET_8(KEEPALIVE_TIME)
FT_W5500_SET_SOCKET_8(KEEPALIVE_TIME)
FT_W5500_GET_SOCKET_8(RX_BUFSIZE)
FT_W5500_SET_SOCKET_8(RX_BUFSIZE)
FT_W5500_GET_SOCKET_16(RX_RECVSIZE)
FT_W5500_GET_SOCKET_16(RX_READPTR)
FT_W5500_SET_SOCKET_16(RX_READPTR)
FT_W5500_GET_SOCKET_16(RX_WRITEPTR)
FT_W5500_GET_SOCKET_8(TX_BUFSIZE)
FT_W5500_SET_SOCKET_8(TX_BUFSIZE)
FT_W5500_GET_SOCKET_16(TX_FREESIZE)
FT_W5500_GET_SOCKET_16(TX_WRITEPTR)
FT_W5500_SET_SOCKET_16(TX_WRITEPTR)


int w5500_socket_write_data(BYTE socketNumber, BYTE *buf, WORD size) {

        CHECK_SOCKET_NUMBER(socketNumber);

        WORD index = 0;

        w5500_get_socket_TX_WRITEPTR(socketNumber, &index);
        w5500_write_mult(index,_bsb.sockets[socketNumber].socket_tx_buffer,buf,size);

        index += size;

        w5500_set_socket_TX_WRITEPTR(socketNumber,index);

}

int w5500_socket_read_data(BYTE socketNumber, BYTE *buf, WORD size) {
        CHECK_SOCKET_NUMBER(socketNumber);
        WORD index = 0;
        w5500_get_socket_RX_READPTR(socketNumber, &index);
        w5500_read_mult(index,_bsb.sockets[socketNumber].socket_rx_buffer,buf,size);
        index += size;
        w5500_set_socket_RX_READPTR(socketNumber, index);
}


/* 
**********************
LOW LEVEL STUFF BELOW HERE
**********************
*/

int w5500_write_byte(int address, BYTE bsb, BYTE data) {
        return w5500_write_mult(address, bsb, &data, 1);
}
int w5500_write_mult(int address, BYTE bsb, BYTE* data, WORD size) { 
        

        _spi_header_buffer[0] = HIGH(address);
        _spi_header_buffer[1] = LOW(address);
        _spi_header_buffer[2] = bsb | W5500_RWB_WRITE | W5500_OMB_VDM;
 
        spi_begin_trans();
        spi_write(_spi_header_buffer, 3);
        spi_write(data, size);
        spi_end_trans();

        return W5500_OK;
}


int w5500_read_mult(int address, BYTE bsb, BYTE* data, WORD size) {
 
        _spi_header_buffer[0] = HIGH(address);
        _spi_header_buffer[1] = LOW(address);
        _spi_header_buffer[2] = bsb | W5500_RWB_READ | W5500_OMB_VDM;

        spi_begin_trans();
        spi_write(_spi_header_buffer, 3);
        spi_read(data, size);
        spi_end_trans();
 
        return W5500_OK;
}
int w5500_read_byte(int address, BYTE bsb, BYTE *data) {
        return w5500_read_mult(address, bsb, data, (WORD) 1);
}

/* 
**********************
DEBUG STUFF
**********************
*/


int w5500_dump_state() {

        BYTE* rbuf = (BYTE*) malloc(32);

        w5500_get_VERSIONR(rbuf);
        printf("W5500 Version = %d\n", rbuf[0]);

        w5500_get_SRCMAC(rbuf);
        printf("MAC Address = %x:%x:%x:%x:%x:%x\n", rbuf[0], rbuf[1], rbuf[2], rbuf[3], rbuf[4], rbuf[5]);

        w5500_get_SRCIP(rbuf);
        printf("IP Address = %d.%d.%d.%d\n", rbuf[0], rbuf[1], rbuf[2], rbuf[3]);

        w5500_get_SUBMASK(rbuf);
        printf("Subnet Mask = %d.%d.%d.%d\n", rbuf[0], rbuf[1], rbuf[2], rbuf[3]);

        w5500_get_GWADDR(rbuf);
        printf("Gateway = %d.%d.%d.%d\n", rbuf[0], rbuf[1], rbuf[2], rbuf[3]);

        w5500_phycfg_t phyConfig;
        w5500_get_PHYCFG(&phyConfig);

        printf("Duplex Status = ");
        switch(phyConfig.duplex_status)
        {
                case W5500_PHYCFG_DUPLEX_HALF: printf("HALF\n"); break;
                case W5500_PHYCFG_DUPLEX_FULL: printf("FULL\n"); break;
                default : printf("UNKNOWN\n"); break;
        }

        printf("Link Status = ");
        switch(phyConfig.link_status)
        {
                case W5500_PHYCFG_LINKSTATUS_DOWN: printf("DOWN\n"); break;
                case W5500_PHYCFG_LINKSTATUS_UP: printf("UP\n"); break;
                default : printf("UNKNOWN\n"); break;
        }

        printf("Speed = ");
        switch(phyConfig.speed_status)
        {
                case W5500_PHYCFG_SPEED_10MBPS: printf("10Mbps\n"); break;
                case W5500_PHYCFG_SPEED_100MBPS: printf("100Mbps\n"); break;
                default : printf("UNKNOWN\n"); break;
        }

        printf("OpMode = ");
        switch(phyConfig.opmode)
        {
                case W5500_PHYCFG_OPMODE_10BT_HALF_NOAUTO : printf("10BT HALF NOAUTO\n"); break;
                case W5500_PHYCFG_OPMODE_10BT_FULL_NOAUTO  : printf("10BT FULL NOAUTO\n"); break;
                case W5500_PHYCFG_OPMODE_100BT_HALF_NOAUTO  : printf("100BT HALF NOAUTO\n"); break;
                case W5500_PHYCFG_OPMODE_100BT_FULL_NOAUTO  : printf("100BT FULL NOAUTO\n"); break;
                case W5500_PHYCFG_OPMODE_100BT_HALF_AUTO  : printf("100BT HALF AUTO\n"); break;
                case W5500_PHYCFG_OPMODE_NOTUSED  : printf("10BT HALF NOAUTO\n"); break;
                case W5500_PHYCFG_OPMODE_POWERDOWN   : printf("POWERDOWN\n"); break;
                case W5500_PHYCFG_OPMODE_ALL_CAPABLE_AUTO  : printf("ALL CAPABLE\n"); break;
                default : printf("UNKNOWN\n"); break;
        }

        free(rbuf);
}
