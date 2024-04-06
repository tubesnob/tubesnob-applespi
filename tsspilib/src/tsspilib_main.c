#include "tsspilib.h"


void spi_init() {
}

void spi_shutdown() {
}

void spi_begin_trans() {
    SCLK_OFF;
    SSEL_OFF;
}

void spi_end_trans() {
    SSEL_ON;
    SCLK_OFF;
}

int spi_write(uint8_t *txbuf, uint16_t txsize) {
    return spisendb(txbuf, txsize);
}

int spi_read(uint8_t* rxbuf, uint16_t rxsize) {
    return spireadb(rxbuf, rxsize);
}

