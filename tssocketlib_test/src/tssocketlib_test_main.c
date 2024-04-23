#include <stdio.h>

#include "tssocketlib_test.h"

#ifdef __MACOS__
#include "../../tsspilib_driver_ftdi/src/tsspilib_driver_ftdi.h"
#endif

#ifdef __APPLE2GS__
#include "../../tsspilib_driver_a2gpio/src/tsspilib_driver_a2gpio.h"
#endif

int main(int argc, char** argv)
{
        int counter;
        int counter2;

        uint8_t hwaddr[]   = { 0x80, 0x70, 0x60, 0x50, 0x40, 0x30 };
        uint8_t ipaddr[]   = { 10, 0, 0, 133 };
        uint8_t mask[]     = { 255, 255, 255, 0 };
        uint8_t gwaddr[]   = { 10, 0, 0, 1};

        counter = 0;
        tslog_init();
        
        _tslog->info("Initializing SPI\n");
        tsspilib_device_vtbl_t* spi_device = NULL;
    
        #ifdef __APPLE2GS__
        spi_device = a2gpio_spi_driver_load();
        #else
        spi_device = ftdi_spi_driver_load();
        #endif
    
        if (spi_device==NULL) {
            printf("No device loaded\n");
            return SPI_ERROR;
        }
        spi_init(spi_device);

    
        _tslog->info("Initializing W5500\n");
        w5500_init();

        _tslog->info("Resetting W5500\n");
        w5500_reset();

        //waitMilliseconds(1000);

//while (1) {

//        _tslog->info("Setting MAC\n");
        w5500_set_SRCMAC(hwaddr);
//}

        _tslog->info("Setting Source IP\n");
        w5500_set_SRCIP(ipaddr);

        _tslog->info("Setting Subnet Mask\n");
        w5500_set_SUBMASK(mask);

        _tslog->info("Setting Gateway Address\n");
        w5500_set_GWADDR(gwaddr);
    
        w5500_dump_state();
        uint8_t rbuf[] = { 0,0,0,0,0,0 };
     
        _tslog->info("Creating Socket\n");
        socket_t* socket = socket_create(0x36, 8044);

        waitMilliseconds(1000);

        // connect to www.google.com
        socket->dest_ip.a0 = 104;
        socket->dest_ip.a1 = 26;
        socket->dest_ip.a2 = 8;
        socket->dest_ip.a3 = 177;
        socket->dest_port = 80;

        _tslog->info("Connecting\n");
        socket->connect(socket);

        waitSeconds(2);
        dump_socketStatus(socket);

        uint16_t tempSize = 0x0400;

        uint8_t *sendBuffer = (uint8_t*) malloc(tempSize);
        uint8_t *receiveBuffer = (uint8_t*) malloc(tempSize);

        sprintf((char*)sendBuffer,"GET / HTTP/1.1\nHost: www.blabbermouth.net\nUser-Agent: AppleIIgsSPI\nAccept: text/html\nAccept-Language: en-us,en;\n\n\n");

        _tslog->info("sending...\n");
        socket->send(socket, sendBuffer, strlen((const char*)sendBuffer));

        waitSeconds(2);

        uint16_t receiveubytesAvailable = 0;

        clock_t cStart;
        clock_t cEnd;
        time_t tStart;
        time_t tEnd;

        cStart = clock();
        time(&tStart);

        int totalBytesReceived = 0;
        do {
                socket->receive_available(socket,&receiveubytesAvailable);
                if (receiveubytesAvailable>tempSize) receiveubytesAvailable = tempSize-1;
                memset(receiveBuffer,0,tempSize);
                socket->receive(socket, receiveBuffer, receiveubytesAvailable);
                totalBytesReceived += receiveubytesAvailable;
        } while (receiveubytesAvailable);

 
        cEnd = clock();
        time(&tEnd);

        double elapsedSeconds2 = ((double) cEnd - cStart) / 60.0F;
        double uint8_tsPerSecond = ( (double)totalBytesReceived / elapsedSeconds2);
        _tslog->info("Total Elapsed = %f seconds. BPS=%f\n",elapsedSeconds2,uint8_tsPerSecond);
        _tslog->info("Total uint8_ts Received = %i\n",totalBytesReceived);

        return 0;

        int longCount = 10000;
        while(longCount--) {

                memset(sendBuffer,0,tempSize);

                for(int bufpos = 0; bufpos < (tempSize-1); bufpos++) {
                        sendBuffer[bufpos] = (unsigned char) (65+((longCount+bufpos)%26));
                }
                _tslog->info("countdown=%i\n",longCount);

                clock_t cStart;
                clock_t cEnd;
                time_t tStart;
                time_t tEnd;

                cStart = clock();
                time(&tStart);
                socket->send(socket, sendBuffer, strlen((const char*)sendBuffer));
                cEnd = clock();
                time(&tEnd);

                double elapsedSeconds2 = ((double) cEnd - cStart) / 60.0F;
                double elapsedSeconds = difftime(tEnd, tStart);
                double uint8_tsPerSecond = ( (double)tempSize / elapsedSeconds2);
                _tslog->info("Total Elapsed = %f seconds. BPS=%f\n",elapsedSeconds2,uint8_tsPerSecond);

                uint16_t receiveubytesAvailable = 0;
                memset(receiveBuffer,0,tempSize);

                socket->receive_available(socket,&receiveubytesAvailable);

                _tslog->info("There are %i uint8_ts available to read\n", receiveubytesAvailable);
                if (receiveubytesAvailable>tempSize) receiveubytesAvailable = tempSize-1;
                socket->receive(socket, receiveBuffer, receiveubytesAvailable);
                _tslog->info("Received Data : %s\n", receiveBuffer);

                waitSeconds(3);



        }

    
        waitSeconds(60);

}



