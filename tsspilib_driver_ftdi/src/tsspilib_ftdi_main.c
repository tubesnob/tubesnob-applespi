#include "tsspilib_ftdi.h"

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
static int ftdi_spi_init() {

    int ret;

    if ((__ftdi = ftdi_new()) == 0)
    {
        printf("FTDI: Could not allocate new FTDI context\n");
        return SPI_NODEVICE;
    }
 
    __ftdi_version = ftdi_get_library_version();
    printf("FTDI: Initialized libftdi %s (major: %d, minor: %d, micro: %d, snapshot ver: %s)\n",__ftdi_version.version_str, __ftdi_version.major, __ftdi_version.minor, __ftdi_version.micro,__ftdi_version.snapshot_str);

    if ((ret = ftdi_usb_open(__ftdi, USB_MAJOR, USB_MINOR)) < 0)
    {
        printf("FTDI: unable to open ftdi device @ (%X:%X): %d (%s)\n", USB_MAJOR, USB_MINOR, ret, ftdi_get_error_string(__ftdi));
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

    ret = ftdi_usb_reset(__ftdi);
    FTDI_HANDLE_ERROR_FATAL(ret,"USB RESET", SPI_ERROR)

    ret = ftdi_set_interface(__ftdi, INTERFACE_ANY);
    FTDI_HANDLE_ERROR_FATAL(ret,"SET INTERFACE", SPI_ERROR)

    ret = ftdi_set_bitmode(__ftdi, 0, 0); // reset
    FTDI_HANDLE_ERROR_FATAL(ret,"RESET DEVICE", SPI_ERROR)
    
    ret = ftdi_set_bitmode(__ftdi, 0, BITMODE_MPSSE); // enable mpsse on all bits
    FTDI_HANDLE_ERROR_FATAL(ret,"ENABLE MPSSE", SPI_ERROR)

    ret = ftdi_usb_purge_buffers(__ftdi);
    FTDI_HANDLE_ERROR_FATAL(ret,"PURGE BUFFERS", SPI_ERROR)

    usleep(50000); // sleep 50 ms for setup to complete

    // Set these pins high
    // everything should be LOW except for SSEL
    const unsigned char pinInitialState = PIN_SSEL;
   
    // Use these pins as outputs
    const unsigned char pinDirection = PIN_SCLK | PIN_MOSI | PIN_SSEL;


    // set all pins to output, and then set pin 3 (0x08) to input
    __ftdi_pins[0] = 0xFF;
    CLEAR_PIN(PIN_MISO);
    ret = ftdi_set_bitmode(__ftdi, __ftdi_pins[0], BITMODE_BITBANG );
    if (ret < 0) {
        printf("FTDI: Could not enable bitbang mode. %d\n", ret);
        return SPI_NODEVICE;
    }

    // set everything low
    __ftdi_pins[0] = 0x00;
    ftdi_write_data(__ftdi, __ftdi_pins, 1);

    return SPI_OK;
}

static int ftdi_spi_shutdown() {
    if (__ftdi != NULL) {
        ftdi_usb_close(__ftdi);
        __ftdi = NULL;
    }
    return SPI_OK;
}

/*
    uint8_t pins[0]; \
    ftdi_read_pins(__ftdi, (unsigned char*) pins); \
    if (pins[0] != __ftdi_pins[0]) { \
        printf("FTDI: Read of pins after %s set returned 0x%X but we expected 0x%X\n", "PIN_" #__name__, pins[0], __ftdi_pins[0]); \
    } \
    __ftdi_pins[0] = pins[0]; \

    uint8_t pins[0]; \
    ftdi_read_pins(__ftdi, (unsigned char*) pins); \
    if (pins[0] != __ftdi_pins[0]) { \
        printf("FTDI: Read of pins after %s clear returned 0x%X but we expected 0x%X\n", "PIN_" #__name__, pins[0], __ftdi_pins[0]); \
    } \
    __ftdi_pins[0] = pins[0]; \

    printf("FTDI: Setting %s with 0x%X\n", "PIN_"#__name__,__ftdi_pins[0]); \
    printf("FTDI: Done Setting %s with 0x%X hr=0x%X\n", "PIN_"#__name__,__ftdi_pins[0],hr); \

    printf("FTDI: Clearing %s with 0x%X\n", "PIN_"#__name__,__ftdi_pins[0]); \
    printf("FTDI: Done Clearing %s with 0x%X hr=0x%X\n", "PIN_"#__name__,__ftdi_pins[0],hr); \


*/


#define DEFFUNC_FTDI_SET_PIN(__name__)      \
    int __name__##_SET() {      \
    SET_PIN(PIN_##__name__); \
    int hr = ftdi_write_data(__ftdi, (unsigned char*) __ftdi_pins, 1); \
    if (hr < 0) { \
        printf("FTDI: Failed to set pin 0x%X [0x%X] err=[0x%X]\n", PIN_##__name__,__ftdi_pins[0], hr); \
        return SPI_ERROR; \
    } \
    return SPI_OK; } 

#define DEFFUNC_FTDI_CLEAR_PIN(__name__) \
    int __name__##_CLEAR() { \
    CLEAR_PIN(PIN_##__name__); \
    int hr = ftdi_write_data(__ftdi, (unsigned char*) __ftdi_pins, 1); \
    if (hr < 0) { \
        printf("FTDI: Failed to clear pin 0x%X [0x%X] err=[0x%X]\n", PIN_##__name__,__ftdi_pins[0],hr); \
        return SPI_ERROR; \
    } \
    return SPI_OK; } 

DEFFUNC_FTDI_SET_PIN(SCLK)
DEFFUNC_FTDI_SET_PIN(SSEL)
DEFFUNC_FTDI_SET_PIN(MOSI)
DEFFUNC_FTDI_CLEAR_PIN(SCLK)
DEFFUNC_FTDI_CLEAR_PIN(SSEL)
DEFFUNC_FTDI_CLEAR_PIN(MOSI)

static int ftdi_spi_begin_trans() {
    SCLK_CLEAR();
    SSEL_CLEAR();
    return SPI_OK;
}

static int ftdi_spi_end_trans() {
    SSEL_SET();
    SCLK_CLEAR();
    return SPI_OK;
}

static int ftdi_spi_write(uint8_t *txbuf, uint16_t txsize) {
    SCLK_CLEAR();
    SSEL_CLEAR();
    uint16_t txcompleted = 0;
    while(txcompleted < txsize) {
        uint8_t buf = txbuf[txcompleted++];
        for(int x=0; x<8; x++) {
            MOSI_CLEAR();
            SCLK_CLEAR();
            if (buf & (1<<x)) {
                MOSI_SET();
            }
            SCLK_SET();
        }
    }
    SSEL_SET();
    SCLK_CLEAR();
    return txcompleted;
}

static int ftdi_spi_read(uint8_t* rxbuf, uint16_t rxsize) {
    SCLK_CLEAR();
    SSEL_CLEAR();
    uint16_t rxcompleted = 0;
    while(rxcompleted < rxsize) {
        uint8_t buf = 0;
        for(int x=0; x<8; x++) {
            uint8_t pins[0];
            SCLK_CLEAR();
            SCLK_SET();
            ftdi_read_pins(__ftdi, pins);
            if (pins[0] & PIN_MISO) {
                buf = buf | (1 << x);
            }
        }
        rxbuf[rxcompleted++] = buf;
    }
    SCLK_CLEAR();
    SSEL_SET();
    return rxcompleted;
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