#include "spidl.h"
#include "../../orcadefaults.h"

#define BUFFER_SIZE 0x1000

typedef struct {
        unsigned long packetNumber;
        unsigned long totalNumberOfPackets;
        unsigned long packetDataLength;
        WORD packetCRC16;
} data_packet_t;

typedef struct
{
        char *source_macaddr;
        char *source_ipaddr;
        char *source_mask;
        char *source_gwaddr;
        int  source_port;

        char *dest_ip;
        int   dest_port;

        char *file_name;
        char *save_file_name;

        WORD packet_size;

} config_t;

static int config_handler(void* user, const char* section, const char* name, const char* value);

int main(int argc, char** argv)
{
        config_t config;

        if (ini_parse("spidl.cfg", config_handler, &config) < 0) {
                DEBUG_LOG("Couldn't find spidl.cfg. Exiting\n");
                return 1;
        }

        if (argc<2) {
                DEBUG_LOG("No parameters specified");
                return 1;
        }

        config.file_name = config.save_file_name = strdup(argv[1]);
        if (argc==3) 
                config.save_file_name = strdup(argv[2]);

        DEBUG_LOG("Source\n   IP : %s\n   MAC : %s\n   Mask : %s\n   GW : %s\n",config.source_ipaddr,config.source_macaddr,config.source_mask,config.source_gwaddr);
        DEBUG_LOG("Destination\n   IP : %s\n   Port : %i\n",config.dest_ip, config.dest_port);
        DEBUG_LOG("Transfer\n   File Name : %s\n   Save File Name : %s\n   Packet Size : %i\n",config.file_name, config.save_file_name, config.packet_size);


        DEBUG_LOG("Initializing SPI ... ");
        spi_init();

        DEBUG_LOG("Initializing W5500 ... ");
        w5500_init();
        DEBUG_LOG("OK\n");


        DEBUG_LOG("Resetting W5500 ... ");
        w5500_reset();
        DEBUG_LOG("OK\n");

        DEBUG_LOG("Setting W5500 LAN Properties ... ");
        address_t source_hwaddr;
        strtomac(config.source_macaddr, &source_hwaddr);
        w5500_set_SRCMAC((BYTE*) source_hwaddr);
        
        address_t source_ipaddr;
        strtoip(config.source_ipaddr, &source_ipaddr);

        w5500_set_SRCIP((BYTE*) source_ipaddr);

        address_t source_mask;
        strtoip(config.source_mask, &source_mask);

        w5500_set_SUBMASK((BYTE*) source_mask);
        
        address_t source_gwaddr;
        strtoip(config.source_gwaddr, &source_gwaddr);

        w5500_set_GWADDR((BYTE*) source_gwaddr);

        DEBUG_LOG("OK\n");

        w5500_phycfg_t phy;
        w5500_get_PHYCFG(&phy);
        printf("PHY = %d\n",phy.link_status);
        
        DEBUG_LOG("Creating Socket ... ");
        socket_t* socket;
        socket_create(SOCKET_PROTOCOL_TCP, config.source_port, &socket);
        DEBUG_LOG("[%d]\n",socket->number);

        strtoip(config.dest_ip, &socket->dest_ip);
        socket->dest_port = config.dest_port;

        DEBUG_LOG("Connecting ...");
        socket->connect(socket);
        
        // wait until we are connected...
        while (socket->status != SOCKET_STATUS_ESTABLISHED)
        {
                DEBUG_LOG(".");
                waitMilliseconds(100);
                socket->refresh(socket);
        }
        DEBUG_LOG(" Connected\n");

        // small buffer for our send commands
        char *sendCommand = (char*) malloc(200);

        // send the command to the destination
        sprintf(sendCommand,"SENDFILE\n%s\n%i\n\0",config.file_name,config.packet_size);
        socket->send(socket, sendCommand, strlen(sendCommand));

        // set up a buffer to receive data...
        BYTE* receiveBuffer = (BYTE*) malloc(BUFFER_SIZE);

        WORD packetHeaderSize = sizeof(data_packet_t);
        WORD fixedReceiveBlockSize = packetHeaderSize + config.packet_size;

        // receive a block of data...
        socket->receive(socket, receiveBuffer, fixedReceiveBlockSize);
        WORD* headerBytes = (WORD*) receiveBuffer;

        //printf("Packet Header Size : %d\n",packetHeaderSize);
        //for(int z=0; z < 16; z++) {
        //        printf("[%d] : %x\n",z,receiveBuffer[z]);        
        //};
        //return;

        data_packet_t* packet = (data_packet_t*) receiveBuffer;
        BYTE* packetData = &receiveBuffer[packetHeaderSize];

        if (packet->packetNumber !=0) {
                DEBUG_LOG("Invalid packet header\nExiting\n");
                return;
        }

        unsigned long totalFileSize = packet->packetDataLength;
        unsigned int totalFileSizeKB = totalFileSize / 1024;

        printf("Total File Size = %u KB\n",totalFileSizeKB);

        FILE* outfile = fopen(config.save_file_name,"w");
        if (!outfile) {
                DEBUG_LOG("Could not open file [%s] for writing\nExiting\n", config.save_file_name);
                return 1;
        }

        clock_t xfer_start = clock();
        DWORD workPacketNumber = 1;
        while(workPacketNumber <= packet->totalNumberOfPackets) {

                sprintf(sendCommand,"sendpacket\n%d\n\0",workPacketNumber);

                DEBUG_LOG(">");
                socket->send(socket, sendCommand, strlen(sendCommand));

                DEBUG_LOG("\b<");
                socket->receive(socket,receiveBuffer,fixedReceiveBlockSize);

                DEBUG_LOG("\bC");
                WORD ourCRC16 = gen_crc16(packetData, packet->packetDataLength);

                if (packet->packetCRC16 != ourCRC16) {
                        workPacketNumber--;
                        DEBUG_LOG("\bX\b");
                }
                else {

                        DEBUG_LOG("\bW");
                        fwrite(packetData, 1, packet->packetDataLength, outfile);
                        DEBUG_LOG("\b*");

                }
                workPacketNumber++;
        }
        clock_t xfer_end = clock();

        DEBUG_LOG("-");
        fclose(outfile);
        DEBUG_LOG("\b+\n");

        sprintf(sendCommand,"quit\n\0");
        socket->send(socket, sendCommand, strlen(sendCommand));

        DEBUG_LOG("Cleaning Up ... ");

        free(receiveBuffer);
        free(sendCommand);
        
        DEBUG_LOG(" Done\n");

        DEBUG_LOG("\nWaiting....\n");
        waitSeconds(1);

        DEBUG_LOG("Done!\n");

        DWORD ticks = xfer_end - xfer_start;
        double totalMilliseconds = 16.6666f * ticks;
        double totalSeconds = totalMilliseconds / 1000;
        double bytesPerSecond = totalFileSize / totalSeconds;
        printf("Total Elapsed Time = %fms\n",totalMilliseconds);
        printf("Bytes Per Second = %f\n",bytesPerSecond);


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



