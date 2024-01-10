#include "tsspilib.h"

#include "../../orcadefaults.h"
#pragma noroot

#define WORD unsigned short 
#define BYTE unsigned char 
#define DWORD unsigned int

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

#define SSEL_OFF (*((BYTE*)A2_AN0_OFF)  = (BYTE)0xA0)
#define SSEL_ON  (*((BYTE*)A2_AN0_ON)   = (BYTE)0xA0)
#define SCLK_OFF (*((BYTE*)A2_AN1_OFF)  = (BYTE)0xA0)
#define SCLK_ON  (*((BYTE*)A2_AN1_ON)   = (BYTE)0xA0)
#define MOSI_OFF (*((BYTE*)A2_AN3_OFF)  = (BYTE)0xA0)
#define MOSI_ON  (*((BYTE*)A2_AN3_ON)   = (BYTE)0xA0)
#define MISO_GET ((BYTE)(*((BYTE*)A2_PB0)))

 unsigned short spisendb(BYTE *buffer, WORD numberOfBytes);
 unsigned short spireadb(BYTE *buffer, WORD numberOfBytes);

void spi_init() {
}

void spi_begin_trans() {
    SCLK_OFF;
    SSEL_OFF;
}

void spi_end_trans() {
    SSEL_ON;
    SCLK_OFF;
}

int spi_write(BYTE *txbuf, WORD txsize) {
    return spisendb(txbuf, txsize);
}

int spi_read(BYTE* rxbuf, WORD rxsize) {
    return spireadb(rxbuf, rxsize);
}

