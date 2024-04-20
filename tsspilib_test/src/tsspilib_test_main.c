#include <stdio.h>
#include "tsspilib_test.h"
#include "../../tsspilib/src/tsspilib.h"

#ifdef __MACOS__
#include "../../tsspilib_driver_ftdi/src/tsspilib_ftdi.h"
#endif

#ifdef __APPLE2GS__
#include "../../tsspilib_driver_a2gpio/src/tsspilib_a2gpio.h"
#endif


int main(int argc, char** argv)
{
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

    spi_shutdown();

    printf("this is a test\n");
    return 0;
}



