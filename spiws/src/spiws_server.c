#include "spiws.h"
#include "../../orcadefaults.h"

#pragma noroot

#define MAX_REQUEST_SIZE        4000
#define REQUEST_BUFFER_SIZE     8192
#define LINE_BUFFER_SIZE        8192
#define RESPONSE_BUFFER_SIZE    8192

#define C_CR 10
#define C_LF 13

uint8_t    __tempBuffer[255];
uint8_t*   __requestBufferPos;
uint8_t*   __streamReaderPos;
uint8_t*   __requestBuffer;
uint8_t*   __lineBuffer;
uint8_t*   __responseBuffer;
uint8_t*   __fileBuffer;

void spiws_server_handle(config_t* config, socket_t* socket);
int  spiws_server_streamreader(socket_t* socket, uint8_t* textBuffer, int size);
int  spiws_server_streamwriter(socket_t* socket, uint8_t* buffer, int size);

int spiws_server_run(config_t* config) {

        __requestBuffer = malloc(REQUEST_BUFFER_SIZE);
        __lineBuffer = malloc(REQUEST_BUFFER_SIZE);
        __responseBuffer = malloc(RESPONSE_BUFFER_SIZE);
        __fileBuffer = malloc(1024);

        __requestBufferPos = __requestBuffer;
        __streamReaderPos = __requestBuffer;

        memset(__requestBuffer,0,REQUEST_BUFFER_SIZE);

        while(1) {

                socket_t* socket = socket_create(SOCKET_PROTOCOL_TCP, config->source_listenport);

                if (socket->listen(socket)) {

                        iptostr(socket->dest_ip, (char*)__tempBuffer);
                        _tslog->info("Connected to %s:%u\n", __tempBuffer,socket->dest_port);
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

        _tslog->info("Handling socket %i[%i]\n",socket->id,socket->number);
        
        int lineCount=0;

        while(1) {

                socket->refresh(socket);

                if (socket->status != SOCKET_STATUS_ESTABLISHED) {
                        _tslog->info("Socket is no longer connected.\n");
                        break;
                }

                int lineLength = spiws_server_streamreader(socket, __lineBuffer, LINE_BUFFER_SIZE);
                if (lineLength) {
                        //_tslog->info("[%u:%u:%u] : %s",lineCount++,lineLength,__lineBuffer[0],__lineBuffer);
                }

                if (lineLength==1) {

                        strcpy((char*) __responseBuffer,"HTTP/1.1 200 OK\nServer: AppleSPI\n\0");
                        spiws_server_streamwriter(socket,__responseBuffer, (int)strlen((char*)__responseBuffer));


                        strcpy((char*)__responseBuffer,"Content-Type: text/html\n");
                        spiws_server_streamwriter(socket,__responseBuffer, (int)strlen((char*)__responseBuffer));

                        
                        FILE* fp = fopen("index.html","r");
                        int fileSize = 0;
                        if (fp) {
                                fseek(fp, 0, SEEK_END); // seek to end of file
                                fileSize = ftell(fp); // get current file pointer
                                fseek(fp, 0, SEEK_SET); // seek back to beginning of file         
                        }
                        else
                                _tslog->info("No file\n");
                        
                        _tslog->info((char*) __responseBuffer,"Content-Length: %u\n",fileSize);
                        spiws_server_streamwriter(socket,__responseBuffer, (int) strlen((char*)__responseBuffer));

                        strcpy((char*)__responseBuffer,"Connection: closed\n");
                        spiws_server_streamwriter(socket,__responseBuffer, strlen((char*)__responseBuffer));

                        strcpy((char*)__responseBuffer,"\n\n");
                        spiws_server_streamwriter(socket,__responseBuffer, strlen((char*)__responseBuffer));

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

                        strcpy((char*)__responseBuffer,"\n\n");
                        spiws_server_streamwriter(socket,__responseBuffer, strlen((char*)__responseBuffer));

                        break;

                }

        }

}

int spiws_server_streamreader(socket_t* socket, uint8_t *linebuffer, int size) {

        uint8_t* end = &__requestBuffer[REQUEST_BUFFER_SIZE];

        if (socket->rx_bytes_available) {
                //_tslog->info("reading %u bytes\n",socket->rx_bytes_available);
                if (__requestBufferPos + socket->rx_bytes_available > end) return 0;
                uint16_t bytesRead = socket->receive(socket, __requestBufferPos, socket->rx_bytes_available);
                //_tslog->info("read %u bytes\n",bytesRead);
                __requestBufferPos += bytesRead;
        }

        /*
        uint8_t* xxx = __streamReaderPos;
        uint8_t counter = 0;
        while(xxx < __requestBufferPos && counter < 10) {
                _tslog->info("[%u]",*xxx);
                xxx++;
                counter++;
        }
        _tslog->info("\n***************\n");
        */

        // do we have any data to process?
        if (__streamReaderPos >= __requestBufferPos) return 0;

        uint8_t *tp = __streamReaderPos;
        uint8_t c = *tp;

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
                uint16_t length = tp - __streamReaderPos;
                memcpy(__lineBuffer, __streamReaderPos, length);
                __lineBuffer[length] = 0x00;
                __streamReaderPos = tp;
                __streamReaderPos++;
                return length;
        }
        return 0;
}


int  spiws_server_streamwriter(socket_t* socket, uint8_t* buffer, int size) {
        //_tslog->info("[SEND] %s\n",buffer);
        socket->send(socket,buffer,size);
        return size;
}






