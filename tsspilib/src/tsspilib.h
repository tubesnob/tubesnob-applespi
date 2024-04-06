#include "../../orcadefaults.h"

#ifndef __TSSPILIB_DOT_H__
#define __TSSPILIB_DOT_H__

typedef struct tsspilib_device_vtbl {
   int     (*tsspilib_device_init)();
   int     (*tsspilib_device_shutdown)();
   int     (*tsspilib_device_begin_trans)();
   int     (*tsspilib_device_end_trans)();
   int     (*tsspilib_device_write)(uint8_t* txbuf, uint16_t txsize);
   int     (*tsspilib_device_read)(uint8_t* rxbuf, uint16_t rzsize);
} tsspilib_device_vtbl_t;

void spi_init(tsspilib_device_vtbl_t* spidev);
void spi_shutdown();
void spi_begin_trans();
void spi_end_trans();
int  spi_write(uint8_t* txbuf, uint16_t txsize);
int  spi_read(uint8_t* rxbuf, uint16_t rxsize);

#endif
