#ifdef __APPLE2GS__
segment "AUTOSEG~~~";
#endif

#include <stdlib.h>
#include <stdio.h>
#include "w5500_raw_socket.h"
#include "w5500.h"
#include "w5500_defs.h"
#include "../../tslib/src/tslib.h"
#include "../../tsspilib/src/tsspilib.h"
#include "../../orcadefaults.h"

static w5500_raw_socket_t       __sockets[W5500_MAX_SOCKETS];
static unsigned int             __socket_id = 0;        // running list of socket numbers. used to provide each with a unique ID over time

static int w5500_raw_socket_close(w5500_raw_socket_t* socket);
static int w5500_raw_socket_send(w5500_raw_socket_t *socket, unsigned char *buf, unsigned short length);
static int w5500_raw_socket_receive(w5500_raw_socket_t *socket, unsigned char *buf, unsigned short length);
static int w5500_raw_socket_refresh(w5500_raw_socket_t *socket);
static int w5500_raw_socket_receive_available(w5500_raw_socket_t *socket, unsigned short *size);

void w5500_raw_socket_init() {
      __socket_id = 0;
      for(unsigned short sn=0; sn < W5500_MAX_SOCKETS; sn++) {
            memset(&__sockets[sn],0,sizeof(w5500_raw_socket_t));
      }      
}

w5500_raw_socket_t* w5500_raw_socket_create() {

    // close all the sockets since MACRAW requires socket #0 and no other sockets to be running
    for(int sn=0; sn < W5500_MAX_SOCKETS; sn++) {
        w5500_set_socket_COMMAND(sn, W5500_SOCKET_CMD_CLOSE);
    }
    int rawSocketNumber = 0;
    w5500_raw_socket_t* ss = (w5500_raw_socket_t*) malloc(sizeof(w5500_raw_socket_t));
    ss->id = __socket_id++;
    ss->number = 0;
    ss->protocol = W5500_SOCKET_MODE_MACRAW; 
    ss->close = &w5500_raw_socket_close;
    ss->send = &w5500_raw_socket_send;
    ss->receive_available = &w5500_raw_socket_receive_available;
    ss->receive = &w5500_raw_socket_receive;
    ss->refresh = &w5500_raw_socket_refresh;
    w5500_set_socket_RX_BUFSIZE(ss->number, 0x10);
    w5500_set_socket_TX_BUFSIZE(ss->number, 0x10);
    w5500_set_socket_MODE(ss->number, W5500_SOCKET_MODE_MACRAW | 0b11110000);
    w5500_set_socket_COMMAND(ss->number, W5500_SOCKET_CMD_OPEN);
    return ss;
}

static int w5500_raw_socket_close(w5500_raw_socket_t* socket)
{
      return w5500_set_socket_COMMAND(socket->number, W5500_SOCKET_CMD_CLOSE);
}

static int w5500_raw_socket_refresh(w5500_raw_socket_t *socket) {
      w5500_get_socket_STATUS(socket->number, &socket->status);
      w5500_get_socket_RX_RECVSIZE(socket->number, &socket->rx_bytes_available);
      return SOCKET_OK;
}


/*
***********************************
SEND FUNCTIONS
***********************************
*/


static int w5500_raw_socket_send(w5500_raw_socket_t *socket, unsigned char *buf, unsigned short length) {

      unsigned short txBufferFreeSize = 0;
      unsigned short bytesLeftToSend = length;
      unsigned char socketInterruptStatus = 0;
      unsigned char socketStatus = 0;
      unsigned char *bufpos = buf;

      while (bytesLeftToSend) {

            // check the socket status. if it is not established, we cannot send data
            w5500_get_socket_STATUS(socket->number, &socketStatus);
            if (socketStatus != W5500_SOCKET_STATUS_ESTABLISHED && socket->protocol != W5500_SOCKET_MODE_MACRAW) {
                  return SOCKET_ERR_INVALIDARGUMENT;
            }

            // wait until we get the send ok signal from the socket
            while ((!socketInterruptStatus) & W5500_SOCKET_INTERRUPT_SENDOK) {
                  w5500_get_socket_INTERRUPT(socket->number, &socketInterruptStatus);
            }

            // we are clear to send ; get the free size available in the tx buffer...
            w5500_get_socket_TX_FREESIZE(socket->number, &txBufferFreeSize);

            // if there is no room in the buffer... loop until we have some space free
            if (!txBufferFreeSize) {
                  continue;
            }

            unsigned short bytesToSendThisPass = bytesLeftToSend;
            if (bytesLeftToSend > txBufferFreeSize)
                  bytesToSendThisPass = txBufferFreeSize;

            w5500_socket_write_data(socket->number, bufpos, bytesToSendThisPass);

            w5500_set_socket_COMMAND(socket->number, W5500_SOCKET_CMD_SEND);

            bytesLeftToSend -= bytesToSendThisPass;
            bufpos += bytesToSendThisPass;

      }

      return SOCKET_OK;
}

static int w5500_raw_socket_receive_available(w5500_raw_socket_t *socket, unsigned short *size) {
      w5500_get_socket_RX_RECVSIZE(socket->number, size);
      return SOCKET_OK;
}

static int w5500_raw_socket_receive(w5500_raw_socket_t *socket, unsigned char *buf, unsigned short length) {


      unsigned short socketBytesAvailable = 0;
      unsigned short bytesLeft = length;
      unsigned char socketInterruptStatus = 0;
      unsigned char socketStatus = 0;
      unsigned char *bufpos = buf;

      while (bytesLeft) {

            // check the socket status. if it is not established, we cannot send data
            w5500_get_socket_STATUS(socket->number, &socketStatus);


            if (socketStatus != W5500_SOCKET_STATUS_ESTABLISHED && socket->protocol != W5500_SOCKET_MODE_MACRAW) {
                  return SOCKET_ERR_INVALIDARGUMENT;
            }

            // wait until we get the send ok signal from the socket
            while ((!socketInterruptStatus) & W5500_SOCKET_INTERRUPT_RECV) {
                  w5500_get_socket_INTERRUPT(socket->number, &socketInterruptStatus);
            }

            // we are clear to receive ; get the free size available in the tx buffer...
            w5500_get_socket_RX_RECVSIZE(socket->number, &socketBytesAvailable);

            // if there is no room in the buffer... loop until we have some space free
            if (!socketBytesAvailable) {
                  continue;
            }

            unsigned short bytesToRead = socketBytesAvailable;
            if (bytesToRead > bytesLeft)
                  bytesToRead = bytesLeft;

            unsigned short rxPtr = 0;
            w5500_get_socket_RX_READPTR(socket->number, &rxPtr);
            w5500_socket_read_data(socket->number, bufpos, bytesToRead);
            w5500_set_socket_COMMAND(socket->number, W5500_SOCKET_CMD_RECV);

            w5500_get_socket_RX_READPTR(socket->number, &rxPtr);

            bytesLeft -= bytesToRead;
            bufpos += bytesToRead;



      }



      return length - bytesLeft;
}

/*
***********************************
DEBUG FUNCTIONS
***********************************
*/


void w5500_raw_dump_socketStatus(w5500_raw_socket_t* socket)
{
        unsigned char socketStatus = 0;
        w5500_get_socket_STATUS(socket->number, &socketStatus);
        _tslog->info("Socket [%i]\n",socket->number);
        _tslog->info("  STATUS = ");
        switch(socketStatus) {
                case W5500_SOCKET_STATUS_CLOSED:      _tslog->info("CLOSED\n"); break;
                case W5500_SOCKET_STATUS_INIT:        _tslog->info("INIT\n"); break;
                case W5500_SOCKET_STATUS_LISTEN:      _tslog->info("LISTEN\n"); break;
                case W5500_SOCKET_STATUS_SYNSENT:     _tslog->info("SYN_SENT\n"); break;
                case W5500_SOCKET_STATUS_SYNRECV:     _tslog->info("SYN_RECEIVED\n"); break;
                case W5500_SOCKET_STATUS_ESTABLISHED: _tslog->info("ESTABLISHED\n"); break;
                case W5500_SOCKET_STATUS_FIN_WAIT:    _tslog->info("FIN_WAIT\n"); break;
                case W5500_SOCKET_STATUS_CLOSING:     _tslog->info("CLOSING\n"); break;
                case W5500_SOCKET_STATUS_TIME_WAIT:   _tslog->info("TIME_WAIT\n"); break;
                case W5500_SOCKET_STATUS_CLOSE_WAIT:  _tslog->info("CLOSE_WAIT\n"); break;
                case W5500_SOCKET_STATUS_LAST_ACK:    _tslog->info("LAST_ACK\n"); break;
                case W5500_SOCKET_STATUS_UDP:         _tslog->info("UDP\n"); break;
                case W5500_SOCKET_STATUS_IPRAW:       _tslog->info("IPRAW\n"); break;
                case W5500_SOCKET_STATUS_MACRAW:      _tslog->info("MACRAW\n"); break;
                case W5500_SOCKET_STATUS_PPPOE:       _tslog->info("PPPOE\n"); break;
                default : _tslog->info("UNKNOWN [%x]\n",socketStatus); break;
        }

        unsigned char protocol;
        w5500_get_socket_MODE(socket->number, &protocol);
        _tslog->info("  MODE = %x\n",protocol);

}



