#include "../../orcadefaults.h"

#ifndef __TSSPILIB_DOT_H__
#define __TSSPILIB_DOT_H__

#define SPI_OK          0
#define SPI_ERROR       0x8000
#define SPI_NODEVICE    0x8001

typedef struct tsspilib_device_vtbl {
   int     (*tsspilib_device_init)();
   int     (*tsspilib_device_shutdown)();
   int     (*tsspilib_device_begin_trans)();
   int     (*tsspilib_device_end_trans)();
   int     (*tsspilib_device_write)(uint8_t* txbuf, uint16_t txsize);
   int     (*tsspilib_device_read)(uint8_t* rxbuf, uint16_t rzsize);
} tsspilib_device_vtbl_t;

int spi_init(tsspilib_device_vtbl_t* spidev);
int spi_shutdown();
int spi_begin_trans();
int spi_end_trans();
int spi_write(uint8_t* txbuf, uint16_t txsize);
int spi_read(uint8_t* rxbuf, uint16_t rxsize);

#endif
