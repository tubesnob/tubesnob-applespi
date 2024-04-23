#pragma noroot

#include <stdio.h>
#include <stdlib.h>

#include "../../tslib/src/tslib.h"
#include "tssocketlib.h"
#include "tssocketlib_w5500.h"

static socket_t   __sockets[W5500_MAX_SOCKETS];
static unsigned int      __socket_id = 0;        // running list of socket numbers. used to provide each with a unique ID over time


static int socket_close(socket_t* socket);
static int socket_connect(socket_t* socket);
static int socket_disconnect(socket_t* socket);
static int socket_listen(socket_t* socket);
static int socket_send(socket_t *socket, unsigned char *buf, unsigned short length);
static int socket_receive_available(socket_t *socket, unsigned short *size);
static int socket_receive(socket_t *socket, unsigned char *buf, unsigned short length);
static int socket_refresh(socket_t *socket);



void socket_init() {
      __socket_id = 0;
      for(unsigned short sn=0; sn < W5500_MAX_SOCKETS; sn++) {
            memset(&__sockets[sn],0,sizeof(socket_t));
      }      
}

socket_t* socket_create(unsigned char protocol, unsigned short source_port)
{

   unsigned char sn = 0;
   unsigned char status = 0;

   // find a socket that is closed ....
   for(sn=0; sn < W5500_MAX_SOCKETS; sn++) {
      _tslog->info("Getting status of socket %i : ",sn);
      w5500_get_socket_STATUS(sn, &status);
      _tslog->info("%X\n",status);
      if (status==W5500_SOCKET_STATUS_CLOSED) 
         break;
   };

   // if we went past our maximum number of sockets, none are available
   // clean up any sockets eligible for closure
   if (sn >= W5500_MAX_SOCKETS) {
         _tslog->info("No sockets available. Trying to close one.\n");
         unsigned char found = 0;
         unsigned char status;
         for(sn=0; sn < W5500_MAX_SOCKETS && !found; sn++) {

            _tslog->info("Checking closed status of socket %i : ");

            w5500_get_socket_STATUS(sn, &status);
            switch(status) {
                  case W5500_SOCKET_STATUS_LAST_ACK:
                  case W5500_SOCKET_STATUS_TIME_WAIT:
                  case W5500_SOCKET_STATUS_FIN_WAIT:
                  case W5500_SOCKET_STATUS_CLOSING:
                        _tslog->info("CAN BE CLOSED! CLOSING! ... ");
                        w5500_set_socket_COMMAND(sn, W5500_SOCKET_CMD_CLOSE);
                        found = 1;
                        _tslog->info("CLOSED");
                        break;
                  default:
                        _tslog->info("N/A\n");

            }
         }

         _tslog->info("Socket scan complete. Found = %i. SN = %i\n",found,sn);
   }

   if (sn >= W5500_MAX_SOCKETS) {
      _tslog->info("NO SOCKETS AVAILABLE ERROR!");
      return NULL;
   }

   socket_t* ss = (socket_t*) malloc(sizeof(socket_t));
   ss->id = __socket_id++;
   ss->number = sn;
   ss->source_port = source_port;
   ss->protocol = protocol; 

   //_tslog->info("New socket created at %X with id:%i number:%i sourceport:%i protocol:%i\n", ss, ss->id, ss->number, ss->source_port, ss->protocol);

   ss->connect = &socket_connect;
   ss->close = &socket_close;
   ss->disconnect = &socket_disconnect;
   ss->listen = &socket_listen; 
   ss->send = &socket_send;
   ss->receive_available = &socket_receive_available;
   ss->receive = &socket_receive;
   ss->refresh = &socket_refresh;

   //_tslog->info("Mid assignment - socket created at %X with id:%i number:%i sourceport:%i protocol:%i\n", ss, ss->id, ss->number, ss->source_port, ss->protocol);
   w5500_set_socket_SRCPORT(ss->number, ss->source_port);
   w5500_set_socket_MODE(ss->number, W5500_SOCKET_MODE_TCP);
   w5500_set_socket_COMMAND(ss->number, W5500_SOCKET_CMD_OPEN);

   //_tslog->info("Final assignment - socket created at %X with id:%i number:%i sourceport:%i protocol:%i\n", ss, ss->id, ss->number, ss->source_port, ss->protocol);

   return ss;
   
} 

static int socket_close(socket_t* socket)
{
      return w5500_set_socket_COMMAND(socket->number, W5500_SOCKET_CMD_CLOSE);
}

static int socket_connect(socket_t* socket) {
      w5500_set_socket_DESTIP(socket->number, (unsigned char*) &socket->dest_ip);
      w5500_set_socket_DESTPORT(socket->number, socket->dest_port);
      w5500_set_socket_COMMAND(socket->number, W5500_SOCKET_CMD_CONNECT);
      socket_refresh(socket);
      return SOCKET_OK;
}

static int socket_refresh(socket_t *socket) {
      w5500_get_socket_STATUS(socket->number, &socket->status);
      w5500_get_socket_RX_RECVSIZE(socket->number, &socket->rx_bytes_available);
      return SOCKET_OK;
}

static int socket_disconnect(socket_t* socket) {
      w5500_set_socket_COMMAND(socket->number, W5500_SOCKET_CMD_DISCON);
      return SOCKET_OK;
}

/*
***********************************
SEND FUNCTIONS
***********************************
*/


static int socket_send(socket_t *socket, unsigned char *buf, unsigned short length) {

      unsigned short txBufferFreeSize = 0;
      unsigned short bytesLeftToSend = length;
      unsigned char socketInterruptStatus = 0;
      unsigned char socketStatus = 0;
      unsigned char *bufpos = buf;

      while (bytesLeftToSend) {

            // check the socket status. if it is not established, we cannot send data
            w5500_get_socket_STATUS(socket->number, &socketStatus);
            if (socketStatus != W5500_SOCKET_STATUS_ESTABLISHED) {
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

/*
***********************************
RECEIVE FUNCTIONS
***********************************
*/


static int socket_receive_available(socket_t *socket, unsigned short *size) {
      w5500_get_socket_RX_RECVSIZE(socket->number, size);
      return SOCKET_OK;
}

static int socket_receive(socket_t *socket, unsigned char *buf, unsigned short length) {


      unsigned short socketBytesAvailable = 0;
      unsigned short bytesLeft = length;
      unsigned char socketInterruptStatus = 0;
      unsigned char socketStatus = 0;
      unsigned char *bufpos = buf;

      while (bytesLeft) {

            // check the socket status. if it is not established, we cannot send data
            w5500_get_socket_STATUS(socket->number, &socketStatus);
            if (socketStatus != W5500_SOCKET_STATUS_ESTABLISHED) {
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
LISTEN FUNCTIONS
***********************************
*/


static int socket_listen(socket_t* socket) {
      _tslog->info("Listening on port %i\n",socket->source_port);

      w5500_set_socket_COMMAND(socket->number, W5500_SOCKET_CMD_LISTEN);

      unsigned char socketStatus=0;
      while(1) {
            w5500_get_socket_STATUS(socket->number, &socketStatus);
            if (socketStatus==W5500_SOCKET_STATUS_ESTABLISHED) {
                  _tslog->info("Connected.\n");
                  break;
            }




      }

      w5500_get_socket_DESTIP(socket->number, (unsigned char*) &socket->dest_ip);
      w5500_get_socket_DESTPORT(socket->number, &socket->dest_port);

      return 1;

}


/*
***********************************
UTILITY FUNCTIONS
***********************************
*/


char* iptostr(address_t ip, char* buf) {
      sprintf(buf, "%i.%i.%i.%i", ip.a0, ip.a1, ip.a2, ip.a3);
      return buf;
}

int strtoip(const char *str, address_t *ip) {
      int a0, a1, a2, a3;
      sscanf(str,"%i.%i.%i.%i", &a0, &a1, &a2, &a3);
      ip->a0 = (unsigned char) a0;
      ip->a1 = (unsigned char) a1;
      ip->a2 = (unsigned char) a2;
      ip->a3 = (unsigned char) a3;
      ip->a4 = 0x00;
      ip->a5 = 0x00;
      return 0;
}

char* mactostr(address_t mac, char *buf) {
      _tslog->info(buf, "%x:%x:%x:%x:%x:%x\0", mac.a0, mac.a1, mac.a2, mac.a3, mac.a4, mac.a5);
      return buf;
}

int strtomac(const char *str, address_t *mac) {
      int a0, a1, a2, a3, a4, a5;
      sscanf(str,"%x:%x:%x:%x:%x:%x", &a0, &a1, &a2, &a3, &a4, &a5);
      mac->a0 = (unsigned char) a0;
      mac->a1 = (unsigned char) a1;
      mac->a2 = (unsigned char) a2;
      mac->a3 = (unsigned char) a3;
      mac->a4 = (unsigned char) a4;
      mac->a5 = (unsigned char) a5;
      return 0;
}

address_t* addrcpy(address_t* dest, const address_t* source) {
      if (!source) return NULL;
      if (!dest) return NULL;
      memcpy((unsigned char*) dest, (unsigned char*) source, sizeof(address_t));      
      return dest;
}

/*
***********************************
DEBUG FUNCTIONS
***********************************
*/


void dump_socketStatus(socket_t* socket)
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

        unsigned short source_port;
        w5500_get_socket_SRCPORT(socket->number, &source_port);
        _tslog->info("  SOURCE PORT = %i\n",source_port);

        address_t dest_ip;
        w5500_get_socket_DESTIP(socket->number, (unsigned char*) &dest_ip);
        
        char ipBuf[32];
        iptostr(dest_ip, ipBuf);
        _tslog->info("  DEST IP = %s\n",ipBuf);
        
        unsigned short dest_port;
        w5500_get_socket_DESTPORT(socket->number, &dest_port);
        _tslog->info("  DEST PORT = %i\n",dest_port);

}



