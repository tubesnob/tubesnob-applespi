#include "tsspilib.h"

#define A2_AN0_OFF  0xc058
#define A2_AN0_ON   0xc059
#define A2_AN1_OFF  0xc05a
#define A2_AN1_ON   0xc05b
#define A2_AN2_OFF  0xc05c
#define A2_AN2_ON   0xc05d
#define A2_AN3_OFF  0xc05e
#define A2_AN3_ON   0xc05f
#define A2_PB3      0xc060
#define A2_PB0      0xc061
#define A2_PB1      0xc062
#define A2_PB2      0xc063
#define A2_PDL1X    0xc064
#define A2_PDL1Y    0xc065
#define A2_PDL2X    0xc066
#define A2_PDL2Y    0xc067

#define SSEL_OFF (*((uint8_t*)A2_AN0_OFF)  = (uint8_t)0xA0)
#define SSEL_ON  (*((uint8_t*)A2_AN0_ON)   = (uint8_t)0xA0)
#define SCLK_OFF (*((uint8_t*)A2_AN1_OFF)  = (uint8_t)0xA0)
#define SCLK_ON  (*((uint8_t*)A2_AN1_ON)   = (uint8_t)0xA0)
#define MOSI_OFF (*((uint8_t*)A2_AN3_OFF)  = (uint8_t)0xA0)
#define MOSI_ON  (*((uint8_t*)A2_AN3_ON)   = (uint8_t)0xA0)
#define MISO_GET ((uint8_t)(*((uint8_t*)A2_PB0)))

 uint16_t spisendb(uint8_t *buffer, uint16_t numberOfuint8_ts);
 uint16_t spireadb(uint8_t *buffer, uint16_t numberOfuint8_ts);

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

