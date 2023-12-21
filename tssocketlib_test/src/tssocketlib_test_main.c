#include <stdio.h>

#include "tssocketlib_test.h"
#include "../../tslib/lib/tslib.h"
#include "../../tsspilib/lib/tsspilib.h"
#include "../../tssocketlib/lib/tssocketlib.h"
#include "../../tssocketlib/lib/tssocketlib_w5500.h"

#include "../../orcadefaults.h"


int main(int argc, char** argv)
{
        int counter;
        int counter2;

        unsigned char hwaddr[]   = { 0x80, 0x70, 0x60, 0x50, 0x40, 0x30 };
        unsigned char ipaddr[]   = { 192, 168, 100, 133 };
        unsigned char mask[]     = { 255, 255, 255, 0 };
        unsigned char gwaddr[]   = { 192, 168, 100, 254};

        counter = 0;
        
        DEBUG_LOG("Initializing SPI\n");
        spi_init();

        DEBUG_LOG("Initializing W5500\n");
        w5500_init();

        DEBUG_LOG("Resetting W5500\n");
        w5500_reset();

        waitMilliseconds(1000);

        DEBUG_LOG("Setting MAC\n");
        w5500_set_SRCMAC(hwaddr);

        DEBUG_LOG("Setting Source IP\n");
        w5500_set_SRCIP(ipaddr);

        DEBUG_LOG("Setting Subnet Mask\n");
        w5500_set_SUBMASK(mask);

        DEBUG_LOG("Setting Gateway Address\n");
        w5500_set_GWADDR(gwaddr);

        unsigned char rbuf[] = { 0,0,0,0,0,0 };
     
        socket_t* socket = NULL;

        DEBUG_LOG("Creating Socket\n");
        socket_create(0x36, 8044, &socket);

        waitMilliseconds(1000);

        // connect to www.google.com
        socket->dest_ip.a0 = 172;
        socket->dest_ip.a1 = 217;
        socket->dest_ip.a2 = 3;
        socket->dest_ip.a3 = 196;
        socket->dest_port = 80;

        DEBUG_LOG("Connecting\n");
        socket->connect(socket);

        waitSeconds(2);
        dump_socketStatus(socket);

        WORD tempSize = 0x0400;

        unsigned char *sendBuffer = (unsigned char*) malloc(tempSize);
        unsigned char *receiveBuffer = (unsigned char*) malloc(tempSize);

        sprintf((char*)sendBuffer,"GET /?a=1&b=2 HTTP/1.1\nHost: www.google.com\nUser-Agent: AppleIIgsSPI\nAccept: text/html\nAccept-Language: en-us,en;\n\n\n");

        DEBUG_LOG("sending...\n");
        socket->send(socket, sendBuffer, strlen((const char*)sendBuffer));

        waitSeconds(2);

        WORD receiveBytesAvailable = 0;

        clock_t cStart;
        clock_t cEnd;
        time_t tStart;
        time_t tEnd;

        cStart = clock();
        time(&tStart);

        int totalBytesReceived = 0;
        do {
                socket->receive_available(socket,&receiveBytesAvailable);
                if (receiveBytesAvailable>tempSize) receiveBytesAvailable = tempSize-1;
                memset(receiveBuffer,0,tempSize);
                socket->receive(socket, receiveBuffer, receiveBytesAvailable);
                totalBytesReceived += receiveBytesAvailable;
        } while (receiveBytesAvailable);

 
        cEnd = clock();
        time(&tEnd);

        double elapsedSeconds2 = ((double) cEnd - cStart) / 60.0F;
        double bytesPerSecond = ( (double)totalBytesReceived / elapsedSeconds2);
        DEBUG_LOG("Total Elapsed = %f seconds. BPS=%f\n",elapsedSeconds2,bytesPerSecond);
        DEBUG_LOG("Total Bytes Received = %d\n",totalBytesReceived);

        return;

        int longCount = 10000;
        while(longCount--) {

                memset(sendBuffer,0,tempSize);

                for(int bufpos = 0; bufpos < (tempSize-1); bufpos++) {
                        sendBuffer[bufpos] = (unsigned char) (65+((longCount+bufpos)%26));
                }
                DEBUG_LOG("countdown=%d\n",longCount);

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
                double bytesPerSecond = ( (double)tempSize / elapsedSeconds2);
                DEBUG_LOG("Total Elapsed = %f seconds. BPS=%f\n",elapsedSeconds2,bytesPerSecond);

                WORD receiveBytesAvailable = 0;
                memset(receiveBuffer,0,tempSize);

                socket->receive_available(socket,&receiveBytesAvailable);

                DEBUG_LOG("There are %d bytes available to read\n", receiveBytesAvailable);
                if (receiveBytesAvailable>tempSize) receiveBytesAvailable = tempSize-1;
                socket->receive(socket, receiveBuffer, receiveBytesAvailable);
                DEBUG_LOG("Received Data : %s\n", receiveBuffer);

                waitSeconds(3);



        }
        waitSeconds(60);

}



