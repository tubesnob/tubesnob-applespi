#ifdef __APPLE2GS__
segment "AUTOSEG~~~";
#endif

#include "lwip_test.h"

int initializeW5500() {
   
    _tslog->info("Initializing W5500\n");
    w5500_init();

    _tslog->info("Resetting W5500\n");
    w5500_reset();

    _tslog->info("Setting PHY opmode to ALL CAPABLE\n");
    w5500_set_phyopmode(W5500_PHYCFG_OPMODE_100BT_HALF_NOAUTO);

    _tslog->info("W5500: Setting MAC\n");
    w5500_set_SRCMAC(_hwaddr);

    //_tslog->info("W5500: Setting Source IP\n");
    //w5500_set_SRCIP(_ipaddr);

    //_tslog->info("W5500: Setting Subnet Mask\n");
    //w5500_set_SUBMASK(_mask);

    //_tslog->info("W5500: Setting Gateway Address\n");
    //w5500_set_GWADDR(_gwaddr);

    w5500_dump_state();
    return 0;
}

