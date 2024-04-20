#include "tsspilib_ftdi.h"
#include <stdlib.h>


static int ftdi_spi_init_dummy() {
    return SPI_OK;
}

static int ftdi_spi_shutdown_dummy() {
    return SPI_OK;
}

static int ftdi_spi_begin_trans_dummy() {
    return SPI_OK;
}

static int ftdi_spi_end_trans_dummy() {
    return SPI_OK;
}

static int ftdi_spi_write_dummy(uint8_t *txbuf, uint16_t txsize) {
    return txsize;
}

static int ftdi_spi_read_dummy(uint8_t* rxbuf, uint16_t rxsize) {
    return rxsize;
}


tsspilib_device_vtbl_t* ftdi_spi_driver_load() {
    tsspilib_device_vtbl_t* rv = (tsspilib_device_vtbl_t*) malloc(sizeof(tsspilib_device_vtbl_t));
    rv->tsspilib_device_init = &ftdi_spi_init_dummy;
    rv->tsspilib_device_shutdown = &ftdi_spi_shutdown_dummy;
    rv->tsspilib_device_begin_trans = &ftdi_spi_begin_trans_dummy;
    rv->tsspilib_device_end_trans = &ftdi_spi_end_trans_dummy;
    rv->tsspilib_device_write = &ftdi_spi_write_dummy;
    rv->tsspilib_device_read = &ftdi_spi_read_dummy;
    return rv;
}






