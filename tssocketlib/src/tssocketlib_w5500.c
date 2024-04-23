#pragma noroot

#include "tssocketlib_w5500.h"
#include "tssocketlib_w5500_defs.h"
#include "../../tslib/src/tslib.h"
#include "../../tsspilib/src/tsspilib.h"
#include "../../orcadefaults.h"

w5500_bsb_t _bsb;

unsigned char _spi_header_buffer[3];

#define CHECK_SOCKET_NUMBER(__x_socket_number__) { if (__x_socket_number__ >= W5500_MAX_SOCKETS) return W5500_ERR_INVALIDSOCKETNUMBER; }

int w5500_write_byte(int address, unsigned char bsb, unsigned char data);
int w5500_read_byte(int address, unsigned char bsb, unsigned char *data);
int w5500_write_mult(int address, unsigned char bsb, unsigned char* data, WORD size);
int w5500_read_mult(int address, unsigned char bsb, unsigned char* data, WORD size);

int w5500_init() {

   _bsb.common_register = 0x00;
   for(int socketNumber=0; socketNumber < W5500_MAX_SOCKETS; socketNumber++) {
      _bsb.sockets[socketNumber].socket_register  = ((socketNumber << 2) | 0x01) << 3;
      _bsb.sockets[socketNumber].socket_tx_buffer = ((socketNumber << 2) | 0x02) << 3;
      _bsb.sockets[socketNumber].socket_rx_buffer = ((socketNumber << 2) | 0x03) << 3;
   }

   w5500_reset();

        //w5500_phycfg_t phycfg;
        //phycfg.opmode_set = 1;
        //phycfg.opmode = W5500_PHYCFG_OPMODE_ALL_CAPABLE_AUTO;
        //w5500_set_PHYCFG(phycfg);

   for(int socketNumber=0; socketNumber < W5500_MAX_SOCKETS; socketNumber++) {
        w5500_set_socket_INTERRUPT(socketNumber, 0xFF);         // reset the interrupt register
        w5500_set_socket_INTMASK(socketNumber, 0xFF);           // set the interrupt mask register to trigger all interrupts
        w5500_set_socket_TX_BUFSIZE(socketNumber, 0x02);        // set the tx buffer to 2k
        w5500_set_socket_RX_BUFSIZE(socketNumber, 0x02);        // set the rx buffer to 2k
        w5500_set_socket_MODE(socketNumber, W5500_SOCKET_MODE_CLOSE);
        w5500_set_socket_COMMAND(socketNumber, W5500_SOCKET_CMD_DISCON); // make sure all sockets are closed
        w5500_set_socket_COMMAND(socketNumber, W5500_SOCKET_CMD_CLOSE); // make sure all sockets are closed
   }

   return W5500_OK;
}

int w5500_close() {
   spi_end_trans();
   return W5500_OK;
}

int w5500_reset() {
        unsigned char mr = 0x80;
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


int w5500_socket_write_data(unsigned char socketNumber, unsigned char *buf, WORD size) {

        CHECK_SOCKET_NUMBER(socketNumber);

        WORD index = 0;

        w5500_get_socket_TX_WRITEPTR(socketNumber, &index);
        w5500_write_mult(index,_bsb.sockets[socketNumber].socket_tx_buffer,buf,size);

        index += size;

        w5500_set_socket_TX_WRITEPTR(socketNumber,index);

        return size;

}

int w5500_socket_read_data(unsigned char socketNumber, unsigned char *buf, WORD size) {
        CHECK_SOCKET_NUMBER(socketNumber);
        WORD index = 0;
        w5500_get_socket_RX_READPTR(socketNumber, &index);
        w5500_read_mult(index,_bsb.sockets[socketNumber].socket_rx_buffer,buf,size);
        index += size;
        w5500_set_socket_RX_READPTR(socketNumber, index);
        return size;
}


/* 
**********************
LOW LEVEL STUFF BELOW HERE
**********************
*/

int w5500_write_byte(int address, unsigned char bsb, unsigned char data) {
        return w5500_write_mult(address, bsb, &data, 1);
}
int w5500_write_mult(int address, unsigned char bsb, unsigned char* data, WORD size) { 
        

        _spi_header_buffer[0] = HIGH(address);
        _spi_header_buffer[1] = LOW(address);
        _spi_header_buffer[2] = bsb | W5500_RWB_WRITE | W5500_OMB_VDM;
 
        spi_begin_trans();

        WORD outbufsize = 3 + size;
        uint8_t* outbuf = (uint8_t*) malloc(outbufsize);
        if (outbuf) {
            memcpy(outbuf,_spi_header_buffer, 3);
            memcpy(&outbuf[3], data, size);
            spi_write(outbuf,outbufsize);
            free(outbuf);
        }
        spi_end_trans();

        return W5500_OK;
}


int w5500_read_mult(int address, unsigned char bsb, unsigned char* data, WORD size) {
 
        _spi_header_buffer[0] = HIGH(address);
        _spi_header_buffer[1] = LOW(address);
        _spi_header_buffer[2] = bsb | W5500_RWB_READ | W5500_OMB_VDM;

        spi_begin_trans();
        spi_write(_spi_header_buffer, 3);
        spi_read(data, size);
        spi_end_trans();
 
        return W5500_OK;
}
int w5500_read_byte(int address, unsigned char bsb, unsigned char *data) {
        return w5500_read_mult(address, bsb, data, (WORD) 1);
}

/* 
**********************
DEBUG STUFF
**********************
*/


int w5500_dump_state() {

        unsigned char* rbuf = (unsigned char*) malloc(32);

        _tslog->info("###### W5500 STATUS DUMP ######\n");
        
        w5500_get_VERSIONR(rbuf);
        _tslog->info("W5500 Version = %d\n", rbuf[0]);

        w5500_get_SRCMAC(rbuf);
        _tslog->info("MAC Address = %x:%x:%x:%x:%x:%x\n", rbuf[0], rbuf[1], rbuf[2], rbuf[3], rbuf[4], rbuf[5]);

        w5500_get_SRCIP(rbuf);
        _tslog->info("IP Address = %d.%d.%d.%d\n", rbuf[0], rbuf[1], rbuf[2], rbuf[3]);

        w5500_get_SUBMASK(rbuf);
        _tslog->info("Subnet Mask = %d.%d.%d.%d\n", rbuf[0], rbuf[1], rbuf[2], rbuf[3]);

        w5500_get_GWADDR(rbuf);
        _tslog->info("Gateway = %d.%d.%d.%d\n", rbuf[0], rbuf[1], rbuf[2], rbuf[3]);

        w5500_phycfg_t phyConfig;
        w5500_get_PHYCFG(&phyConfig);

        _tslog->info("Duplex Status = ");
        switch(phyConfig.duplex_status)
        {
                case W5500_PHYCFG_DUPLEX_HALF: _tslog->info("HALF\n"); break;
                case W5500_PHYCFG_DUPLEX_FULL: _tslog->info("FULL\n"); break;
                default : _tslog->info("UNKNOWN\n"); break;
        }

        _tslog->info("Link Status = ");
        switch(phyConfig.link_status)
        {
                case W5500_PHYCFG_LINKSTATUS_DOWN: _tslog->info("DOWN\n"); break;
                case W5500_PHYCFG_LINKSTATUS_UP: _tslog->info("UP\n"); break;
                default : _tslog->info("UNKNOWN\n"); break;
        }

        _tslog->info("Speed = ");
        switch(phyConfig.speed_status)
        {
                case W5500_PHYCFG_SPEED_10MBPS: _tslog->info("10Mbps\n"); break;
                case W5500_PHYCFG_SPEED_100MBPS: _tslog->info("100Mbps\n"); break;
                default : _tslog->info("UNKNOWN\n"); break;
        }

        _tslog->info("OpMode = ");
        switch(phyConfig.opmode)
        {
                case W5500_PHYCFG_OPMODE_10BT_HALF_NOAUTO : _tslog->info("10BT HALF NOAUTO\n"); break;
                case W5500_PHYCFG_OPMODE_10BT_FULL_NOAUTO  : _tslog->info("10BT FULL NOAUTO\n"); break;
                case W5500_PHYCFG_OPMODE_100BT_HALF_NOAUTO  : _tslog->info("100BT HALF NOAUTO\n"); break;
                case W5500_PHYCFG_OPMODE_100BT_FULL_NOAUTO  : _tslog->info("100BT FULL NOAUTO\n"); break;
                case W5500_PHYCFG_OPMODE_100BT_HALF_AUTO  : _tslog->info("100BT HALF AUTO\n"); break;
                case W5500_PHYCFG_OPMODE_NOTUSED  : _tslog->info("10BT HALF NOAUTO\n"); break;
                case W5500_PHYCFG_OPMODE_POWERDOWN   : _tslog->info("POWERDOWN\n"); break;
                case W5500_PHYCFG_OPMODE_ALL_CAPABLE_AUTO  : _tslog->info("ALL CAPABLE\n"); break;
                default : _tslog->info("UNKNOWN\n"); break;
        }


        for(int socketNumber=0; socketNumber < W5500_MAX_SOCKETS; socketNumber++) {
                _tslog->info("[S%d]: ", socketNumber);
                w5500_get_socket_STATUS(socketNumber, rbuf);
                switch(rbuf[0]) {
                        case W5500_SOCKET_STATUS_CLOSED: _tslog->info("W5500_SOCKET_STATUS_CLOSED\n"); break;
                        case W5500_SOCKET_STATUS_INIT: _tslog->info("W5500_SOCKET_STATUS_INIT\n"); break;
                        case W5500_SOCKET_STATUS_LISTEN: _tslog->info("W5500_SOCKET_STATUS_LISTEN\n"); break;
                        case W5500_SOCKET_STATUS_SYNSENT: _tslog->info("W5500_SOCKET_STATUS_SYNSENT\n"); break;
                        case W5500_SOCKET_STATUS_SYNRECV: _tslog->info("W5500_SOCKET_STATUS_SYNRECV\n"); break;
                        case W5500_SOCKET_STATUS_ESTABLISHED: _tslog->info("W5500_SOCKET_STATUS_ESTABLISHED\n"); break;
                        case W5500_SOCKET_STATUS_FIN_WAIT: _tslog->info("W5500_SOCKET_STATUS_FIN_WAIT\n"); break;
                        case W5500_SOCKET_STATUS_CLOSING: _tslog->info("W5500_SOCKET_STATUS_CLOSING\n"); break;
                        case W5500_SOCKET_STATUS_TIME_WAIT: _tslog->info("W5500_SOCKET_STATUS_TIME_WAIT\n"); break;
                        case W5500_SOCKET_STATUS_CLOSE_WAIT: _tslog->info("W5500_SOCKET_STATUS_CLOSE_WAIT\n"); break;
                        case W5500_SOCKET_STATUS_LAST_ACK: _tslog->info("W5500_SOCKET_STATUS_LAST_ACK\n"); break;
                        case W5500_SOCKET_STATUS_UDP: _tslog->info("W5500_SOCKET_STATUS_UDP\n"); break;
                        case W5500_SOCKET_STATUS_IPRAW: _tslog->info("W5500_SOCKET_STATUS_IPRAW\n"); break;
                        case W5500_SOCKET_STATUS_MACRAW: _tslog->info("W5500_SOCKET_STATUS_MACRAW\n"); break;
                        case W5500_SOCKET_STATUS_PPPOE: _tslog->info("W5500_SOCKET_STATUS_PPPOE\n"); break;
                }
        }

        free(rbuf);

        _tslog->info("###########################\n");

        return 0;
        
}
