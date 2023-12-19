#include "spiws.h";
#include "../../orcadefaults.h"

#pragma noroot

#define MAX_REQUEST_SIZE        4000
#define REQUEST_BUFFER_SIZE     8192
#define LINE_BUFFER_SIZE        8192
#define RESPONSE_BUFFER_SIZE    8192

#define C_CR 10
#define C_LF 13

BYTE    __tempBuffer[255];
BYTE*   __requestBufferPos;
BYTE*   __streamReaderPos;
BYTE*   __requestBuffer;
BYTE*   __lineBuffer;
BYTE*   __responseBuffer;
BYTE*   __fileBuffer;

void spiws_server_handle(config_t* config, socket_t* socket);
int  spiws_server_streamreader(socket_t* socket, BYTE* textBuffer, int size);
int  spiws_server_streamwriter(socket_t* socket, BYTE* buffer, int size);

int spiws_server_run(config_t* config) {

        __requestBuffer = malloc(REQUEST_BUFFER_SIZE);
        __lineBuffer = malloc(REQUEST_BUFFER_SIZE);
        __responseBuffer = malloc(RESPONSE_BUFFER_SIZE);
        __fileBuffer = malloc(1024);

        __requestBufferPos = __requestBuffer;
        __streamReaderPos = __requestBuffer;

        memset(__requestBuffer,0,REQUEST_BUFFER_SIZE);

        while(1) {

                socket_t* socket;
                socket_create(SOCKET_PROTOCOL_TCP, config->source_listenport, &socket);

                if (socket->listen(socket)) {

                        iptostr(socket->dest_ip, __tempBuffer);
                        DEBUG_LOG("Connected to %s:%u\n", __tempBuffer,socket->dest_port);
                }

                spiws_server_handle(config, socket);

                socket->close(socket);
                        
        }

        free(__requestBuffer);
        free(__lineBuffer);
        free(__responseBuffer);
        free(__fileBuffer);

        __requestBuffer = NULL;
        __lineBuffer = NULL;
        __responseBuffer = NULL;
}

void spiws_server_handle(config_t* config, socket_t* socket) {

        DEBUG_LOG("Handling socket %d[%d]\n",socket->id,socket->number);
        
        int lineCount=0;

        while(1) {

                socket->refresh(socket);

                if (socket->status != SOCKET_STATUS_ESTABLISHED) {
                        DEBUG_LOG("Socket is no longer connected.\n");
                        break;
                }

                int lineLength = spiws_server_streamreader(socket, __lineBuffer, LINE_BUFFER_SIZE);
                if (lineLength) {
                        //DEBUG_LOG("[%u:%u:%u] : %s",lineCount++,lineLength,__lineBuffer[0],__lineBuffer);
                }

                if (lineLength==1) {

                        strcpy(__responseBuffer,"HTTP/1.1 200 OK\nServer: AppleSPI\n\0");
                        spiws_server_streamwriter(socket,__responseBuffer, strlen(__responseBuffer));


                        strcpy(__responseBuffer,"Content-Type: text/html\n");
                        spiws_server_streamwriter(socket,__responseBuffer, strlen(__responseBuffer));

                        
                        FILE* fp = fopen("index.html","r");
                        int fileSize = 0;
                        if (fp) {
                                fseek(fp, 0, SEEK_END); // seek to end of file
                                fileSize = ftell(fp); // get current file pointer
                                fseek(fp, 0, SEEK_SET); // seek back to beginning of file         
                        }
                        else
                                DEBUG_LOG("No file\n");
                        
                        sprintf(__responseBuffer,"Content-Length: %u\n",fileSize);
                        spiws_server_streamwriter(socket,__responseBuffer, strlen(__responseBuffer));

                        strcpy(__responseBuffer,"Connection: closed\n");
                        spiws_server_streamwriter(socket,__responseBuffer, strlen(__responseBuffer));

                        strcpy(__responseBuffer,"\n\n");
                        spiws_server_streamwriter(socket,__responseBuffer, strlen(__responseBuffer));

                        if (fp) {
                                while(fileSize) {
                                        int bytesRead = fread(__fileBuffer,1,1024,fp);
                                        fileSize-=bytesRead;
                                        spiws_server_streamwriter(socket, __fileBuffer, bytesRead);
                                        __fileBuffer[bytesRead]=0;
                                }
                        }

                        fclose(fp);
                        fp = NULL;

                        strcpy(__responseBuffer,"\n\n");
                        spiws_server_streamwriter(socket,__responseBuffer, strlen(__responseBuffer));

                        break;

                }

        }

}

int spiws_server_streamreader(socket_t* socket, BYTE *linebuffer, int size) {

        BYTE* end = &__requestBuffer[REQUEST_BUFFER_SIZE];

        if (socket->rx_bytes_available) {
                //DEBUG_LOG("reading %u bytes\n",socket->rx_bytes_available);
                if (__requestBufferPos + socket->rx_bytes_available > end) return 0;
                WORD bytesRead = socket->receive(socket, __requestBufferPos, socket->rx_bytes_available);
                //DEBUG_LOG("read %u bytes\n",bytesRead);
                __requestBufferPos += bytesRead;
        }

        /*
        BYTE* xxx = __streamReaderPos;
        BYTE counter = 0;
        while(xxx < __requestBufferPos && counter < 10) {
                DEBUG_LOG("[%u]",*xxx);
                xxx++;
                counter++;
        }
        DEBUG_LOG("\n***************\n");
        */

        // do we have any data to process?
        if (__streamReaderPos >= __requestBufferPos) return 0;

        BYTE *tp = __streamReaderPos;
        BYTE c = *tp;

        // if we have a null at the position, and there is data ahead of us... ignore it and move on.
        if (c==0) {
                tp++;
                return 0;
        }

        // loop until we find a carriage return, hit the current request buffer pos or reach the end
        while (c != C_CR && tp < __requestBufferPos && tp < end) {
                tp++;
                c = *tp;
        }

        // if we found a carriage return, copy the data into the line buffer and update our stream reader position
        if (c==C_CR) {
                WORD length = tp - __streamReaderPos;
                memcpy(__lineBuffer, __streamReaderPos, length);
                __lineBuffer[length] = 0x00;
                __streamReaderPos = tp;
                __streamReaderPos++;
                return length;
        }
        return 0;
}


int  spiws_server_streamwriter(socket_t* socket, BYTE* buffer, int size) {
        //DEBUG_LOG("[SEND] %s\n",buffer);
        socket->send(socket,buffer,size);
}






