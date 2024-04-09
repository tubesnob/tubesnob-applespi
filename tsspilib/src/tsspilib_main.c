#include "tsspilib.h"

static tsspilib_device_vtbl_t* _spiDevice;

int spi_init(tsspilib_device_vtbl_t* spidev) {
    spi_shutdown();
    _spiDevice = spidev;
    return SPI_OK;
}

int spi_shutdown() {
    int rv = SPI_OK;
    if (_spiDevice) {
        rv = _spiDevice->tsspilib_device_shutdown();
        _spiDevice = NULL;
    }
    return rv;
}

int  spi_begin_trans() {
    if (!_spiDevice)
        return SPI_NODEVICE;
    return _spiDevice->tsspilib_device_begin_trans();
}

int spi_end_trans() {
    if (!_spiDevice)
        return SPI_NODEVICE;
    return _spiDevice->tsspilib_device_end_trans();
}

int spi_write(uint8_t *txbuf, uint16_t txsize) {
    if (!_spiDevice)
        return SPI_NODEVICE;
    return _spiDevice->tsspilib_device_write(txbuf, txsize);
}

int spi_read(uint8_t* rxbuf, uint16_t rxsize) {
    if (!_spiDevice)
        return SPI_NODEVICE;
    return _spiDevice->tsspilib_device_read(rxbuf, rxsize);
}

