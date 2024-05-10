#ifdef __APPLE2GS__
segment "AUTOSEG~~~";
#endif

#include "lwip_test.h"


int main(int argc, char** argv)
{
    printf("Initializing TSLIB ...");
    tslib_init();
    printf("DONE\n");

    printf("Initializing SPI ...");
    initializeSPI();
    printf("DONE\n");
    
    printf("Initializing W5500 ...");
    initializeW5500();
    printf("DONE\n");
    
    printf("Initializing SOCKETS ...");
    socket_init();
    socket_t* socket = socket_create_raw();
    printf("DONE\n");
    
    printf("Initializing LWIP ...");
    initializeLWIP(socket);
    printf("DONE\n");
    
}
