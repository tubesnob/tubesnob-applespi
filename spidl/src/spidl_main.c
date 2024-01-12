#include "spidl.h"

#define BUFFER_SIZE 0x1000

static int config_handler(void* user, const char* section, const char* name, const char* value);
static tsstring_vtbl_t* _strings;

config_t* config_cleanup(config_t* config);

int main(int argc, char** argv)
{
        
        _strings = tsstring_init();

        config_t *config = (config_t*) malloc(sizeof(config_t));
        int __rv;

        if (ini_parse("spidl.cfg", config_handler, config) < 0) {
                printf("Couldn't find spidl.cfg. Exiting\n");
                __rv = 1;
                goto exit;
        }

        if (argc<2) {
                printf("No parameters specified");
                __rv = 1;
                goto exit;
        }

        config->file_name = strdup(argv[1]);
        if (argc==3) {
                config->save_file_name = strdup(argv[2]);
        }
        else
        {
            // use the file name requested as the output file path
            tsstring_t* fileNameString = _strings->create_c(config->file_name);
            tslist_t* fileNameParts = fileNameString->f->split(fileNameString, "/");
            tsstring_t* lastItemInSplitString = (tsstring_t*) fileNameParts->get(fileNameParts, fileNameParts->count-1);
            config->save_file_name = strdup(lastItemInSplitString->data);
            fileNameParts->clear(fileNameParts);
            fileNameString->f->clear(fileNameString);
            lastItemInSplitString->f->clear(lastItemInSplitString);
        }

        printf("Source\n   IP : %s\n   MAC : %s\n   Mask : %s\n   GW : %s\n",config->source_ipaddr,config->source_macaddr,config->source_mask,config->source_gwaddr);
        printf("Destination\n   IP : %s\n   Port : %i\n",config->dest_ip, config->dest_port);
        printf("Transfer\n   File Name : %s\n   Save File Name : %s\n   Packet Size : %i\n",config->file_name, config->save_file_name, config->packet_size);

        printf("Initializing SPI ... ");
        spi_init();
        printf("OK\n");

        printf("Initializing SOCKETS ... ");
        socket_init();
        printf("OK\n");

        printf("Initializing W5500 ... ");
        w5500_init();
        printf("OK\n");

        printf("Resetting W5500 ... ");
        w5500_reset();
        printf("OK\n");

        printf("Setting W5500 LAN Properties ... ");
        address_t* source_hwaddr = (address_t*) malloc(sizeof(address_t));
        strtomac(config->source_macaddr, source_hwaddr);
        w5500_set_SRCMAC((unsigned char*) source_hwaddr);
        
        address_t* source_ipaddr = (address_t*) malloc(sizeof(address_t));
        strtoip(config->source_ipaddr, source_ipaddr);
        w5500_set_SRCIP((unsigned char*) source_ipaddr);

        address_t* source_mask = (address_t*) malloc(sizeof(address_t));
        strtoip(config->source_mask, source_mask);
        w5500_set_SUBMASK((unsigned char*) source_mask);
        
        address_t* source_gwaddr = (address_t*) malloc(sizeof(address_t));
        strtoip(config->source_gwaddr, source_gwaddr);
        w5500_set_GWADDR((unsigned char*) source_gwaddr);
        printf("OK\n");

        printf("Creating Socket ... ");
        socket_t* socket = socket_create(SOCKET_PROTOCOL_TCP, config->source_port);
        printf("[%i]\n",socket->number);

        printf("socket is at address 0x%08X\n", (unsigned int)socket);

        strtoip(config->dest_ip, &socket->dest_ip);
        socket->dest_port = config->dest_port;

        printf("Connecting ...");
        socket->connect(socket);

        // wait until we are connected...
        while (socket->status != SOCKET_STATUS_ESTABLISHED)
        {
                printf(".");
                waitMilliseconds(100);
                socket->refresh(socket);
        }
        printf(" Connected\n");

        // small buffer for our send commands
        char *sendCommand = (char*) malloc(400);

        // send the command to the destination
        sprintf(sendCommand,"SENDFILE\n%s\n%i\n\0",config->file_name,config->packet_size);
        socket->send(socket, (unsigned char *)sendCommand, strlen((const char*)sendCommand));

        // set up a buffer to receive data...
        unsigned char* receiveBuffer = (unsigned char*) malloc(BUFFER_SIZE);

        uint16_t packetHeaderSize = sizeof(data_packet_t);
        uint16_t fixedReceiveBlockSize = packetHeaderSize + config->packet_size;

        // receive a block of data...
        socket->receive(socket, receiveBuffer, fixedReceiveBlockSize);
        uint16_t* headeruint8_ts = (uint16_t*) receiveBuffer;
        
        data_packet_t* packet = (data_packet_t*) receiveBuffer;
        unsigned char* packetData = &receiveBuffer[packetHeaderSize];

        if (packet->packetNumber !=0) {
                printf("Invalid packet header\nExiting\n");
                return;
        }

        unsigned long totalFileSize = packet->packetDataLength;
        unsigned int totalFileSizeKB = totalFileSize / 1024;

        printf("Total File Size = %i KB\n",totalFileSizeKB);

        FILE* outfile = stdout;
        if (config->save_file_name != NULL) {
            outfile = fopen(config->save_file_name,"w");
        }

        if (!outfile) {
                printf("Could not open file [%s] for writing\nExiting\n", config->save_file_name);
                return 1;
        }

        clock_t xfer_start = clock();
        uint16_t workPacketNumber = 1;
        while(workPacketNumber <= packet->totalNumberOfPackets) {

                sprintf(sendCommand,"sendpacket\n%i\n\0",workPacketNumber);

                printf("[%i/%i] : S", workPacketNumber, (unsigned int) packet->totalNumberOfPackets);
                socket->send(socket, (unsigned char*) sendCommand, strlen((const char*)sendCommand));

                printf("R");
                socket->receive(socket,receiveBuffer,fixedReceiveBlockSize);

                printf("C");
                uint16_t ourCRC16 = Nu_CalcCRC16((unsigned short) workPacketNumber, packetData, packet->packetDataLength);
                if (packet->packetCRC16 != ourCRC16) {
                        workPacketNumber--;
                        printf("X");
                }
                else 
                {

                        printf("W");
                        fwrite(packetData, 1, packet->packetDataLength, outfile);
                        printf("*");

                }
                printf("\n");
                workPacketNumber++;
        }

        printf("\nTransfer Completed. Flushing Data.\n");
        fflush(outfile);
        fclose(outfile);
        printf("File Saved\n");

        printf("\nTelling the host we are going away.\n");
        sprintf(sendCommand,"quit\n\0");
        socket->send(socket, (unsigned char*) sendCommand, strlen((const char*)sendCommand));

        printf("Closing Connection\n");
        socket->close(socket);

        
        printf(" Done\n");
        printf("\nWaiting....\n");
        waitSeconds(1);


        clock_t xfer_end = clock();
        uint16_t ticks = xfer_end - xfer_start;
        double totalMilliseconds = 16.6666f * ticks;
        double totalSeconds = totalMilliseconds / 1000;
        double bytesPerSecond = totalFileSize / totalSeconds;
        printf("Total Elapsed Time = %fms\n",totalMilliseconds);
        printf("Bytes Per Second = %f\n",bytesPerSecond);

        exit:

        printf("Cleaning Up ... ");
        config = config_cleanup(config);
        freeandnull(receiveBuffer);
        freeandnull(sendCommand);
        printf("Done!\n");

        return __rv;

}


config_t* config_cleanup(config_t* config) 
{
        if (config) {
                freeandnull(config->dest_ip);
                freeandnull(config->file_name);
                freeandnull(config->save_file_name);
                freeandnull(config->source_gwaddr);
                freeandnull(config->source_ipaddr);
                freeandnull(config->source_macaddr);
                freeandnull(config->source_mask);
                freeandnull(config);
        }
        return NULL;
}

#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
static int config_handler(void* user, const char* section, const char* name, const char* value)
{
    config_t* pconfig = (config_t*) user;

    if (MATCH("source", "ip"))
        pconfig->source_ipaddr = strdup(value);

    if (MATCH("source", "mac"))
        pconfig->source_macaddr = strdup(value);

    if (MATCH("source", "gw"))
        pconfig->source_gwaddr = strdup(value);

    if (MATCH("source", "mask"))
        pconfig->source_mask = strdup(value);

    if (MATCH("source", "port"))
        pconfig->source_port = atoi(value);

    if (MATCH("dest", "ip"))
        pconfig->dest_ip = strdup(value);

    if (MATCH("dest", "port"))
        pconfig->dest_port = atoi(value);

    if (MATCH("transfer", "packetSize"))
        pconfig->packet_size = atoi(value);

    return 1;
}



