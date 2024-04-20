#include "tsspilib_ftdi.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#ifndef __APPLE2GS__
#include <unistd.h>
#include "/usr/local/include/libftdi1/ftdi.h"
#endif


static int ftdi_spi_init() {

    int ret;
    struct ftdi_context *ftdi;
    struct ftdi_version_info version;

    if ((ftdi = ftdi_new()) == 0)
   {
        fprintf(stderr, "ftdi_new failed\n");
        return SPI_NODEVICE;
    }
 
    version = ftdi_get_library_version();
    printf("Initialized libftdi %s (major: %d, minor: %d, micro: %d, snapshot ver: %s)\n",
        version.version_str, version.major, version.minor, version.micro,
        version.snapshot_str);
 
    if ((ret = ftdi_usb_open(ftdi, 0x0403, 0x6014)) < 0)
    {
        fprintf(stderr, "unable to open ftdi device: %d (%s)\n", ret, ftdi_get_error_string(ftdi));
        ftdi_free(ftdi);
        return SPI_NODEVICE;
    }
 
    // Read out FTDIChip-ID of R type chips
    if (ftdi->type == TYPE_R)
    {
        unsigned int chipid;
        printf("ftdi_read_chipid: %d\n", ftdi_read_chipid(ftdi, &chipid));
        printf("FTDI chipid: %X\n", chipid);
    }

     // Read out FTDIChip-ID of R type chips
    if (ftdi->type == TYPE_232H)
    {
        printf("is type 232H\n");
        unsigned int chipid;
        printf("ftdi_read_chipid: %d\n", ftdi_read_chipid(ftdi, &chipid));
        printf("FTDI chipid: %X\n", chipid);
    }
}

static int ftdi_spi_shutdown() {
    return SPI_OK;
}

static int ftdi_spi_begin_trans() {
//    SCLK_OFF;
//    SSEL_OFF;
    return SPI_OK;
}

static int ftdi_spi_end_trans() {
//    SSEL_ON;
//    SCLK_OFF;
    return SPI_OK;
}

static int ftdi_spi_write(uint8_t *txbuf, uint16_t txsize) {
    return txsize;
}

static int ftdi_spi_read(uint8_t* rxbuf, uint16_t rxsize) {
    return rxsize;
}




tsspilib_device_vtbl_t* ftdi_spi_driver_load() {
    tsspilib_device_vtbl_t* rv = (tsspilib_device_vtbl_t*) malloc(sizeof(tsspilib_device_vtbl_t));
    rv->tsspilib_device_init = &ftdi_spi_init;
    rv->tsspilib_device_shutdown = &ftdi_spi_shutdown;
    rv->tsspilib_device_begin_trans = &ftdi_spi_begin_trans;
    rv->tsspilib_device_end_trans = &ftdi_spi_end_trans;
    rv->tsspilib_device_write = &ftdi_spi_write;
    rv->tsspilib_device_read = &ftdi_spi_read;
    return rv;
}