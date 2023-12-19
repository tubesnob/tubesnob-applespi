#ifndef __TSSPILIB_DOT_H__
#define __TSSPILIB_DOT_H__

void spi_init();
void spi_begin_trans();
void spi_end_trans();
int  spi_write(unsigned char* txbuf, unsigned short txsize);
int  spi_read(unsigned char* rxbuf, unsigned short rxsize);

#endif
