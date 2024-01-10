#include "spiws.h";
#include "../../orcadefaults.h"

#pragma noroot

int spiws_init_w5500(config_t* config)
{

        printf("Initializing SPI ... \n");
        spi_init();

        printf("Initializing W5500 ... ");
        w5500_init();
        printf("OK\n");

        printf("Resetting W5500 ... ");
        w5500_reset();
        printf("OK\n");

        printf("Setting W5500 LAN Properties ... ");
        address_t source_hwaddr;
        strtomac(config->source_macaddr, &source_hwaddr);
        w5500_set_SRCMAC((BYTE*) source_hwaddr);
        
        address_t source_ipaddr;
        strtoip(config->source_ipaddr, &source_ipaddr);

        w5500_set_SRCIP((BYTE*) source_ipaddr);

        address_t source_mask;
        strtoip(config->source_mask, &source_mask);

        w5500_set_SUBMASK((BYTE*) source_mask);
        
        address_t source_gwaddr;
        strtoip(config->source_gwaddr, &source_gwaddr);

        w5500_set_GWADDR((BYTE*) source_gwaddr);

        printf("OK\n");

        w5500_phycfg_t phy;
        w5500_get_PHYCFG(&phy);
        printf("PHY = %d\n",phy.link_status);

        return 0;
       
}



