
#ifndef __SPI_DOT_H__
#define __SPI_DOT_H__

typedef unsigned short spi_size_t;

void spi_init();
void spi_begin_trans();
void spi_end_trans();
int  spi_write(unsigned char* txbuf, spi_size_t txsize);
int  spi_read(unsigned char* rxbuf, spi_size_t rxsize);

#endif
