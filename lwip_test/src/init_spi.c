#ifdef __APPLE2GS__
segment "AUTOSEG~~~";
#endif

#include "lwip_test.h"

#ifdef __MACOS__
#include "../../tsspilib_driver_ftdi/src/tsspilib_driver_ftdi.h"
#endif

#ifdef __APPLE2GS__
#include "../../tsspilib_driver_a2gpio/src/tsspilib_driver_a2gpio.h"
#endif

int initializeSPI() {
    _tslog->info("Initializing SPI\n");
    #ifdef __APPLE2GS__
    _spiDevice = a2gpio_spi_driver_load();
    #else
    _spiDevice = ftdi_spi_driver_load();
    #endif
    if (_spiDevice==NULL) {
        printf("No device loaded\n");
        return SPI_ERROR;
    }
    int rv = spi_init(_spiDevice);
    _tslog->info("SPI is initialized\n");
    return rv;
}
