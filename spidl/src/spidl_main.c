#include "spidl.h"
#include "../../tslib/src/tslib.h"
#include "../../tslib/src/getopt.h"
#include "../../tssocketlib/src/tssocketlib_w5500.h"
#include "../../tssocketlib/src/tssocketlib.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#define BUFFER_SIZE 0x1000

static int config_handler(void* user, const char* section, const char* name, const char* value);
config_t* config_cleanup(config_t* config);

int main(int argc, char** argv)
{
    tslib_init();

    int __rv;
    _tslog->logMask = TSLOG_LEVEL_INFO | TSLOG_LEVEL_ERROR;

    char* configFileToLoad = NULL;
    config_t *config = (config_t*) malloc(sizeof(config_t));
    config->outputFileToSTDOUT = 0;

    int c = 0;
    while (1)
    {
        int this_option_optind = optind ? optind : 1;
        c = getopt (argc, argv, "dc:b:vi:o:s");
        if (c == EOF)
            break;
        switch (c)
        {
            case 'd':
                // output file to stdout
                config->outputFileToSTDOUT = 1;
                break;
            case 'c':
                // config file
                configFileToLoad = strdup(optarg);
                break;
            case 'b':
                // block size
                config->packet_size = atoi(optarg);
                break;
            case 'v':
                // verbose
                _tslog->logMask |= TSLOG_LEVEL_VERBOSE;
                break;
            case 'i':
                // file name to read
                config->file_name = strdup(optarg);
                break;
            case 'o':
                // file name to save
                config->save_file_name = strdup(optarg);
                break;
            case 's':
                // silent mode
                _tslog->logMask = TSLOG_LEVEL_NONE;
                break;
        }
    }

    if (!configFileToLoad)
        configFileToLoad = strdup("spidl.cfg");

    if (ini_parse(configFileToLoad, config_handler, config) < 0) {
            _tslog->error("Couldn't find configuration file @ [%s]. Exiting\n",configFileToLoad);
            __rv = 1;
            goto exit;
    }

    if (!config->file_name) {
            _tslog->error("No download file name specified\n");
            __rv = 1;
            goto exit;
    }

    // if the command didn't specify a file name to save, then find the file name in the input file path and use it.
    if (!config->save_file_name) {
        // use the file name requested as the output file path
        tsstring_t* fileNameString = _tsstring->new_c(config->file_name);
        tslist_t* fileNameParts = _tsstring->split(fileNameString, "/");
        tsstring_t* lastItemInSplitString = (tsstring_t*) _tslist->get(fileNameParts, fileNameParts->count-1);
        config->save_file_name = strdup(lastItemInSplitString->data);
        _tslist->clear(fileNameParts);
        _tsstring->clear(fileNameString);
        _tsstring->clear(lastItemInSplitString);
    }

    _tslog->verbose("Source\n   IP : %s\n   MAC : %s\n   Mask : %s\n   GW : %s\n",config->source_ipaddr,config->source_macaddr,config->source_mask,config->source_gwaddr);
    _tslog->verbose("Destination\n   IP : %s\n   Port : %i\n",config->dest_ip, config->dest_port);
    _tslog->verbose("Transfer\n   File Name : %s\n   Save File Name : %s\n   Packet Size : %i\n",config->file_name, config->save_file_name, config->packet_size);

    _tslog->verbose("Initializing SPI ... ");
    spi_init();
    _tslog->verbose("OK\n");

    _tslog->verbose("Initializing SOCKETS ... ");
    socket_init();
    _tslog->verbose("OK\n");

    _tslog->verbose("Initializing W5500 ... ");
    w5500_init();
    _tslog->verbose("OK\n");

    _tslog->verbose("Resetting W5500 ... ");
    w5500_reset();
    _tslog->verbose("OK\n");


    _tslog->info("Initializing Network Adapter ... ");
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
    _tslog->info("OK\n");

    socket_t* socket = socket_create(SOCKET_PROTOCOL_TCP, config->source_port);
    strtoip(config->dest_ip, &socket->dest_ip);
    socket->dest_port = config->dest_port;

    _tslog->info("Connecting to host @ %s:%i ...",config->dest_ip, config->dest_port);
    socket->connect(socket);
    while (socket->status != SOCKET_STATUS_ESTABLISHED)
    {
            _tslog->info(".");
            waitMilliseconds(100);
            socket->refresh(socket);
    }
    _tslog->info(" Connected!\n");

    // small buffer for our send commands and receive buffer
    char *sendCommand = (char*) malloc(400);
    unsigned char* receiveBuffer = (unsigned char*) malloc(BUFFER_SIZE);

    // send the command to the destination
    _tslog->info("Requesting file [%s]\n", config->file_name);
    sprintf(sendCommand,"SENDFILE\n%s\n%i\n\0",config->file_name,config->packet_size);
    socket->send(socket, (unsigned char *)sendCommand, strlen((const char*)sendCommand));

    uint16_t packetHeaderSize = sizeof(data_packet_t);
    uint16_t fixedReceiveBlockSize = packetHeaderSize + config->packet_size;

    // receive a block of data...
    _tslog->info("Waiting for header packet\n");
    socket->receive(socket, receiveBuffer, fixedReceiveBlockSize);
    uint16_t* headeruint8_ts = (uint16_t*) receiveBuffer;
    data_packet_t* packet = (data_packet_t*) receiveBuffer;
    unsigned char* packetData = &receiveBuffer[packetHeaderSize];

    if (packet->packetNumber !=0) {
            _tslog->error("Invalid packet header\nExiting\n");
            return;
    }

    unsigned long totalFileSize = packet->packetDataLength;
    unsigned int totalFileSizeKB = totalFileSize / 1024;
    _tslog->info("Total File Size = %i KB\n",totalFileSizeKB);

    FILE* outfile = stdout;
    if (config->save_file_name != NULL && !config->outputFileToSTDOUT) {
        outfile = fopen(config->save_file_name,"w");
    }

    if (!outfile) {
            _tslog->error("Could not open file [%s] for writing\nExiting\n", config->save_file_name);
            return 1;
    }

    _tslog->info("Starting data transfer\n");
    clock_t xfer_start = clock();
    uint16_t workPacketNumber = 1;
    while(workPacketNumber <= packet->totalNumberOfPackets) {

            sprintf(sendCommand,"sendpacket\n%i\n\0",workPacketNumber);

            _tslog->info("[%i/%i] : S", workPacketNumber, (unsigned int) packet->totalNumberOfPackets);
            socket->send(socket, (unsigned char*) sendCommand, strlen((const char*)sendCommand));

            _tslog->info("R");
            socket->receive(socket,receiveBuffer,fixedReceiveBlockSize);

            _tslog->info("C");
            uint16_t ourCRC16 = Nu_CalcCRC16((unsigned short) workPacketNumber, packetData, packet->packetDataLength);
            if (packet->packetCRC16 != ourCRC16) {
                    workPacketNumber--;
                    _tslog->info("X");
            }
            else 
            {

                    _tslog->info("W");
                    fwrite(packetData, 1, packet->packetDataLength, outfile);
                    _tslog->info("*");

            }
            _tslog->info("\n");
            workPacketNumber++;
    }

    _tslog->info("Data Transfer Completed. Flushing Data And Closing file.\n");
    fflush(outfile);
    fclose(outfile);
    _tslog->info("File Saved\n");

    _tslog->info("Ending Session w/ Host\n");
    sprintf(sendCommand,"quit\n\0");
    socket->send(socket, (unsigned char*) sendCommand, strlen((const char*)sendCommand));

    _tslog->info("Closing Connection\n");
    socket->close(socket);

    clock_t xfer_end = clock();
    uint16_t ticks = xfer_end - xfer_start;
    double totalMilliseconds = 16.6666f * ticks;
    double totalSeconds = totalMilliseconds / 1000;
    double bytesPerSecond = totalFileSize / totalSeconds;
    _tslog->info("Total Elapsed Time = %fms\n",totalMilliseconds);
    _tslog->info("Bytes Per Second = %f\n",bytesPerSecond);

    exit:

    _tslog->info("Cleaning Up ... ");

    config = config_cleanup(config);
    freeandnull(receiveBuffer);
    freeandnull(sendCommand);
    _tslog->info("Done!\n");

    tslib_shutdown();

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



