#include "../../orcadefaults.h"

#ifndef __TSSPILIB_DOT_H__
#define __TSSPILIB_DOT_H__

void spi_init();
void spi_shutdown();
void spi_begin_trans();
void spi_end_trans();
int  spi_write(uint8_t* txbuf, uint16_t txsize);
int  spi_read(uint8_t* rxbuf, uint16_t rxsize);

#endif
