#include "ftditest.h"
#include "../../tslib/src/tslib.h"
#include "../../tslib/src/getopt.h"
#include "../../tsspilib/src/tsspilib.h"
#include "../../tssocketlib/src/tssocketlib_w5500.h"
#include "../../tssocketlib/src/tssocketlib.h"
#include "/usr/local/include/libftdi1/ftdi.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <unistd.h>

#define BUFFER_SIZE 0x1000

int main(int argc, char** argv)
{
    int ret;
    struct ftdi_context *ftdi;
    struct ftdi_version_info version;
    if ((ftdi = ftdi_new()) == 0)
   {
        fprintf(stderr, "ftdi_new failed\n");
        return EXIT_FAILURE;
    }
 
    version = ftdi_get_library_version();
    printf("Initialized libftdi %s (major: %d, minor: %d, micro: %d, snapshot ver: %s)\n",
        version.version_str, version.major, version.minor, version.micro,
        version.snapshot_str);
 
    if ((ret = ftdi_usb_open(ftdi, 0x0403, 0x6014)) < 0)
    {
        fprintf(stderr, "unable to open ftdi device: %d (%s)\n", ret, ftdi_get_error_string(ftdi));
        ftdi_free(ftdi);
        return EXIT_FAILURE;
    }
 
    // Read out FTDIChip-ID of R type chips
    if (ftdi->type == TYPE_R)
    {
        unsigned int chipid;
        printf("ftdi_read_chipid: %d\n", ftdi_read_chipid(ftdi, &chipid));
        printf("FTDI chipid: %X\n", chipid);
    }

     // Read out FTDIChip-ID of R type chips
    if (ftdi->type == TYPE_232H)
    {
        printf("is type 232H\n");
        unsigned int chipid;
        printf("ftdi_read_chipid: %d\n", ftdi_read_chipid(ftdi, &chipid));
        printf("FTDI chipid: %X\n", chipid);

volatile unsigned char buf[0];
uint32_t f;
        ret = ftdi_set_bitmode(ftdi, 0xff, BITMODE_BITBANG );
        if (ret < 0) {
            printf("cannot enable bitbang\n");
        }
        else
        {
            printf("startloop\n");
            while(1) 
            {
                for (int i = 0; i < 100; i++)
                {
                    buf[0] =  0x9;
                    //printf("porta: %02i: 0x%02x \n",i,buf[0]);
                    f = ftdi_write_data(ftdi, buf, 1);
    //                usleep(1 * 10000);
                    
                    f= ftdi_read_pins(ftdi, buf);
//                  printf("*****************\npin4a=%02x\n",buf[0]);

                    buf[0] =  0x8;
                    f = ftdi_write_data(ftdi, buf, 1);
  //                  usleep(1 * 10000);

                    f= ftdi_read_pins(ftdi, buf);
  //                  printf("pin4b=%02x\n",buf[0]);

                    buf[0] =  0xa;
                    f = ftdi_write_data(ftdi, buf, 1);
//                    usleep(1 * 10000);

                    f= ftdi_read_pins(ftdi, buf);
    //                printf("pin4c=%02x\n",buf[0]);

                    buf[0] =  0x0;
                    f = ftdi_write_data(ftdi, buf, 1);
      //              usleep(1 * 10000);

                    f= ftdi_read_pins(ftdi, buf);
        //            printf("pin4d=%02x\n",buf[0]);

          //          usleep(1 * 1000000);
                }
            }
        }
    }
 
    if ((ret = ftdi_usb_close(ftdi)) < 0)
    {
        fprintf(stderr, "unable to close ftdi device: %d (%s)\n", ret, ftdi_get_error_string(ftdi));
        ftdi_free(ftdi);
        return EXIT_FAILURE;
    }
 
    ftdi_free(ftdi);
 
    return EXIT_SUCCESS;
}
