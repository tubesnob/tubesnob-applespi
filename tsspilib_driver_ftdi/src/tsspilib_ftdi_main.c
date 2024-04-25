//#pragma noroot

#include "tsspilib_driver_ftdi.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#ifndef __APPLE2GS__
#include <unistd.h>
#include "/usr/local/include/libftdi1/ftdi.h"
#endif

volatile uint8_t            __ftdi_pins[0];
struct ftdi_context*        __ftdi;
struct ftdi_version_info    __ftdi_version;

#define USB_MAJOR 0x0403
#define USB_MINOR 0x6014

#define CLEAR_PIN(n) __ftdi_pins[0] = __ftdi_pins[0] & ~(n)
#define SET_PIN(n) __ftdi_pins[0] = __ftdi_pins[0] | n
#define PIN_SCLK    0x01
#define PIN_MOSI    0x02
#define PIN_MISO    0x04
#define PIN_SSEL    0x08

#define FTDI_HANDLE_ERROR_FATAL(ftdicode,opstring,retval) if (ftdicode < 0) { printf("FTDI: Error with operation [%s] error=0x%X\n", opstring, ftdicode); return retval; }
#define FTDI_HANDLE_ERROR(ftdicode,opstring) if (ftdicode < 0) { printf("FTDI: Error with operation [%s] error=0x%X\n", opstring, ftdicode); }
#define TOSTRING(x) ""#x

// Set these pins high
// everything should be LOW except for SSEL
const unsigned char __pinInitialState = PIN_SSEL;
   
// Use these pins as outputs
const unsigned char __pinDirection = PIN_SCLK | PIN_MOSI | PIN_SSEL;

static int ftdi_spi_init() {

    int ret;

    printf("FTDI: Creating FTDI Device Context\n");
    if ((__ftdi = ftdi_new()) == 0)
    {
        printf("FTDI: Could not allocate new FTDI context\n");
        return SPI_NODEVICE;
    }
 
    __ftdi_version = ftdi_get_library_version();
    printf("FTDI: Initialized libftdi %s (major: %i, minor: %i, micro: %i, snapshot ver: %s)\n",__ftdi_version.version_str, __ftdi_version.major, __ftdi_version.minor, __ftdi_version.micro,__ftdi_version.snapshot_str);

    printf("FTDI: Opening USB Device [%X:%X]\n", USB_MAJOR, USB_MINOR);
    if ((ret = ftdi_usb_open(__ftdi, USB_MAJOR, USB_MINOR)) < 0)
    {
        printf("FTDI: unable to open ftdi device @ (%X:%X): %i (%s)\n", USB_MAJOR, USB_MINOR, ret, ftdi_get_error_string(__ftdi));
        ftdi_free(__ftdi);
        return SPI_NODEVICE;
    }
    
    // Read out FTDIChip-ID of R type chips
    switch(__ftdi->type) {
        case TYPE_R: printf("FTDI: Chip is TYPE_R\n"); break;
        case TYPE_232H: printf("FTDI: Chip is TYPE_232H\n"); break;
    }
    
    unsigned int chipid;
    ret = ftdi_read_chipid(__ftdi, &chipid);
    FTDI_HANDLE_ERROR_FATAL(ret,"READ CHIPID",SPI_ERROR)
    printf("FTDI: Chip ID is %X\n", chipid);

    printf("FTDI: Resetting USB\n");
    ret = ftdi_usb_reset(__ftdi);
    FTDI_HANDLE_ERROR_FATAL(ret,"USB RESET", SPI_ERROR)

    printf("FTDI: Setting interface to ANY\n");
    ret = ftdi_set_interface(__ftdi, INTERFACE_ANY);
    FTDI_HANDLE_ERROR_FATAL(ret,"SET INTERFACE", SPI_ERROR)

    printf("FTDI: Resetting Device\n");
    ret = ftdi_set_bitmode(__ftdi, 0, 0); // reset
    FTDI_HANDLE_ERROR_FATAL(ret,"RESET DEVICE", SPI_ERROR)
    
    printf("FTDI: Enabling MPSSE on all bits\n");
    ret = ftdi_set_bitmode(__ftdi, 0, BITMODE_MPSSE); // enable mpsse on all bits
    FTDI_HANDLE_ERROR_FATAL(ret,"ENABLE MPSSE", SPI_ERROR)

    printf("FTDI: Purging USB Buffers\n");
    ret = ftdi_usb_purge_buffers(__ftdi);
    FTDI_HANDLE_ERROR_FATAL(ret,"PURGE BUFFERS", SPI_ERROR)

    usleep(50000); // sleep 50 ms for setup to complete


    // Setup MPSSE; Operation code followed by 0 or more arguments.
    unsigned int icmd = 0;
    unsigned char buf[256] = {0};
    buf[icmd++] = TCK_DIVISOR;     // opcode: set clk divisor
    buf[icmd++] = 0x05;            // argument: low bit. 60 MHz / (5+1) = 1 MHz
    buf[icmd++] = 0x00;            // argument: high bit.
    buf[icmd++] = DIS_ADAPTIVE;    // opcode: disable adaptive clocking
    buf[icmd++] = DIS_3_PHASE;     // opcode: disable 3-phase clocking
    buf[icmd++] = SET_BITS_LOW;    // opcode: set low bits (ADBUS[0-7])
    buf[icmd++] = __pinInitialState; // argument: inital pin states
    buf[icmd++] = __pinDirection;    // argument: pin direction
    // Write the setup to the chip.

    printf("FTDI: Setting initial SPI mode\n");
    ret = ftdi_write_data(__ftdi, buf, icmd);
    FTDI_HANDLE_ERROR_FATAL(ret, "SET INITIAL MODE", SPI_ERROR)

    ftdi_usb_purge_tx_buffer(__ftdi);

    return SPI_OK;
}

static int ftdi_spi_shutdown() {
    if (__ftdi != NULL) {
        ftdi_usb_close(__ftdi);
        __ftdi = NULL;
    }
    return SPI_OK;
}

static int ftdi_spi_begin_trans() {
    uint8_t buf[255];
    int cmdpos = 0;
    int retval = 0;
    buf[cmdpos++] = SET_BITS_LOW;
    buf[cmdpos++] = __pinInitialState & ~PIN_SSEL;
    buf[cmdpos++] = __pinDirection;
    ftdi_usb_purge_tx_buffer(__ftdi);
    retval = ftdi_write_data(__ftdi, buf, cmdpos);
    ftdi_usb_purge_tx_buffer(__ftdi);
    FTDI_HANDLE_ERROR_FATAL(retval, "BEGIN TRANS",SPI_ERROR);
    if (retval != cmdpos) {
        printf("FTDI: END_TRANS should have sent %i bytes, but only sent %i instead.\n",cmdpos,retval);
        return SPI_ERROR;
    }
    return SPI_OK;
}

static int ftdi_spi_end_trans() {
    uint8_t buf[255];
    int cmdpos = 0;
    buf[cmdpos++] = SET_BITS_LOW;
    buf[cmdpos++] = __pinInitialState | PIN_SSEL;
    buf[cmdpos++] = __pinDirection;
    ftdi_usb_purge_tx_buffer(__ftdi);
    int retval = ftdi_write_data(__ftdi, buf, cmdpos);
    ftdi_usb_purge_tx_buffer(__ftdi);
    FTDI_HANDLE_ERROR_FATAL(retval, "END TRANS",SPI_ERROR);
    if (retval != cmdpos) {
        printf("FTDI: END_TRANS should have sent %i bytes, but only sent %i instead.\n",cmdpos,retval);
        return SPI_ERROR;
    }
    return SPI_OK;
}

static int ftdi_spi_write(uint8_t *txbuf, uint16_t txsize) {

    uint16_t bufferSize = txsize+3;
    uint8_t *buf = (uint8_t*) malloc(bufferSize);
    if (buf) {

        int cmdpos = 0;
        memset(buf, 0, bufferSize);
        buf[cmdpos++] = MPSSE_DO_WRITE | MPSSE_WRITE_NEG;
        buf[cmdpos++] = (txsize-1)&0xFF;    //  length low byte, 0x0000 ==> 1 byte
        buf[cmdpos++] = (txsize-1)>>8;      // length high byte
        int dpos = 0;
        while(dpos < txsize)
            buf[cmdpos++] = txbuf[dpos++];
        ftdi_usb_purge_tx_buffer(__ftdi);
        int retval = ftdi_write_data(__ftdi, buf, cmdpos);
        FTDI_HANDLE_ERROR_FATAL(retval, "SPI_WRITE:WRITE_DATA",0);
        if (retval != cmdpos) {
            printf("FTDI: SPI_WRITE:WRITE_DATA should have sent %i bytes, but only sent %i instead.\n",cmdpos,retval);
            return 0;
        }
        ftdi_usb_purge_tx_buffer(__ftdi);
        free(buf);
        buf = NULL;
        return txsize;
    }
    return 0;
}

static int ftdi_spi_read(uint8_t* rxbuf, uint16_t rxsize) {

    uint8_t buf[3];

    int cmdpos = 0;
    // commands to write and read one byte in SPI0 (polarity = phase = 0) mode
    buf[cmdpos++] = MPSSE_DO_READ | MPSSE_READ_NEG;
    buf[cmdpos++] = (rxsize-1)&0xFF;    //  length low byte, 0x0000 ==> 1 byte
    buf[cmdpos++] = (rxsize-1)>>8;      // length high byte
    ftdi_usb_purge_tx_buffer(__ftdi);
    int retval = ftdi_write_data(__ftdi, buf, cmdpos);
    FTDI_HANDLE_ERROR_FATAL(retval, "SPI_READ:WRITE_DATA",0);
    if (retval != cmdpos) {
        printf("FTDI: SPI_READ:WRITE_DATA should have sent %i bytes, but only sent %i instead.\n",cmdpos,retval);
        return 0;
    }

    ftdi_usb_purge_tx_buffer(__ftdi);
    retval = ftdi_read_data(__ftdi, rxbuf, rxsize);
    FTDI_HANDLE_ERROR_FATAL(retval,"SPI_READ:READ_DATA",0)
    if (retval != rxsize) {
            printf("FTDI: SPI_READ:READ_DATA should have read %i bytes, but only read %i instead.\n",rxsize,retval);
            return 0;
    }
    return rxsize;
}

tsspilib_device_vtbl_t* ftdi_spi_driver_load() {
    tsspilib_device_vtbl_t* rv = (tsspilib_device_vtbl_t*) malloc(sizeof(tsspilib_device_vtbl_t));
    rv->tsspilib_device_init = &ftdi_spi_init;
    rv->tsspilib_device_shutdown = &ftdi_spi_shutdown;
    rv->tsspilib_device_begin_trans = &ftdi_spi_begin_trans;
    rv->tsspilib_device_end_trans = &ftdi_spi_end_trans;
    rv->tsspilib_device_write = &ftdi_spi_write;
    rv->tsspilib_device_read = &ftdi_spi_read;
    return rv;
}