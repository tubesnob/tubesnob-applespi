#ifdef __APPLE2GS__
segment "AUTOSEG~~~";
#endif

#include <stdio.h>
#include "lwip_test.h"

/* lwIP core includes */
#include "lwip/opt.h"
#include "lwip/sys.h"
#include "lwip/timeouts.h"
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/init.h"
#include "lwip/tcpip.h"
#include "lwip/netif.h"
#include "lwip/api.h"
#include "lwip/pbuf.h"

#include "lwip/snmp.h"
#include "lwip/tcp.h"
#include "lwip/udp.h"
#include "lwip/dns.h"
#include "lwip/dhcp.h"
#include "lwip/autoip.h"

/* lwIP netif includes */
#include "lwip/etharp.h"
#include "netif/ethernet.h"

typedef struct {
    unsigned char       dsta[6];
    unsigned char       srca[6];
    uint16_t            type;
} ethheader_t;

struct ip_globals       ip_data;
struct netif            _netif;
unsigned char           *_mac_send_buffer;
unsigned char           *_mac_read_buffer;
socket_t*               _socket;

err_t ts_netif_init(struct netif *netif);
err_t ts_netif_output(struct netif *netif, struct pbuf *p);

err_t ts_netif_output(struct netif *netif, struct pbuf *p)
{
    printf("LWIP: NETIF_OUTPUT\n");
    /* Update SNMP stats (only if you use SNMP) */
    pbuf_copy_partial(p, _mac_send_buffer, p->tot_len, 0);
    _socket->send(_socket, _mac_send_buffer, p->tot_len);
    printf("LWIP: NETIF_OUTPUT_DONE\n");
    return ERR_OK;
}

void ts_netif_status_callback(struct netif *netif)
{
    printf("LWIP NETIF STATUS CALLBACK : %s\n", ip4addr_ntoa(netif_ip4_addr(netif)));
}

err_t ts_netif_init(struct netif *netif)
{    
    printf("LWIP: NETIF_INIT\n");
    netif->linkoutput = ts_netif_output;
    netif->output     = etharp_output; 
    netif->mtu        = 1500; // DEFAULT VALUE? ETHERNET_MTU;
    netif->flags      = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET | NETIF_FLAG_IGMP | NETIF_FLAG_MLD6;
    printf("LWIP: NETIF_INIT DONE\n");
    return ERR_OK;
}

int initializeLWIP(socket_t* socket) {

    printf("Initializing LWIP with socket @ [%04X]", socket);
    _socket = socket;

    printf("LWIP: Allocating MAC Read Buffer .... ");
    _mac_read_buffer = malloc(16384);
    printf("DONE [%04X]\n", _mac_read_buffer);

    printf("LWIP: Allocating MAC Send Buffer .... ");
    _mac_send_buffer = malloc(16384);
    printf("DONE [%04X]\n", _mac_send_buffer);

    printf("LWIP: LWIP_INIT ... ");
    lwip_init();
    printf("DONE\n");

    printf("LWIP: PBUF INIT ... ");
    pbuf_init();
    printf("DONE\n");

    ip4_addr_t ipAddress;
    ipAddress.addr = 0x8600000A;
    ip4_addr_t subnetMask;
    subnetMask.addr = 0x00FFFFFF;
    ip4_addr_t gwAddress;
    gwAddress.addr = 0x010000A0;

    printf("LWIP: NETIF ADD ... ");
    netif_add(&_netif, &ipAddress, &subnetMask, &gwAddress, NULL, ts_netif_init, netif_input);
    printf("DONE\n");

    _netif.hostname = "test";
    _netif.name[0] = 'e';
    _netif.name[1] = '0';

    SMEMCPY(_netif.hwaddr, _hwaddr, 6);
    _netif.hwaddr_len = 6;
    
    printf("LWIP: SET STATUS CALLBACK\n");
    netif_set_status_callback(&_netif, ts_netif_status_callback);

    printf("LWIP: SET DEFAULT\n");
    netif_set_default(&_netif);

    printf("LWIP: SET UP\n");
    netif_set_up(&_netif);

    printf("LWIP: SET LINK UP\n");
    netif_set_link_up(&_netif);
    
    printf("LWIP: STARTING DHCP\n");
    err_t x = dhcp_start(&_netif );
    printf("LWIP: STARTING DHCP DONE\n");
    
    while(1) {

        printf("LWIP: MAINLOOP\n");

        _socket->refresh(_socket);
        int bytesToRead = _socket->rx_bytes_available;
        while(bytesToRead) {
            int bytesRead = _socket->receive(_socket, _mac_read_buffer, bytesToRead);
            unsigned char *pp = (unsigned char *) &_mac_read_buffer;
            while (pp < &_mac_read_buffer[bytesRead]) {
                uint16_t frameSize = ((pp[0]<<8) | pp[1]) -2;
                if (frameSize > 1510) {
                    printf("LWIP - BAD FRAME SIZE [%04X]\n", frameSize); 
                    goto fastExit;
                }
                pp+=2;
                ethheader_t *ethframe = (ethheader_t*) pp;
                printf("ethframe:%i/%i dest=[%X:%X:%X:%X:%X:%X] src=[%X:%X:%X:%X:%X:%X] type=[%X]\n",frameSize, bytesRead, 
                    ethframe->dsta[0], ethframe->dsta[1], ethframe->dsta[2], ethframe->dsta[3], ethframe->dsta[4], ethframe->dsta[5], 
                    ethframe->srca[0], ethframe->srca[1], ethframe->srca[2], ethframe->srca[3], ethframe->srca[4], ethframe->srca[5], 
                    ethframe->type);

                struct pbuf* buffer = pbuf_alloc(PBUF_RAW, frameSize, PBUF_POOL);
                pbuf_take(buffer, pp, frameSize);
                if(_netif.input(buffer, &_netif) != ERR_OK) {
                    printf("asd\n");
                }
                pp += frameSize;
            }
            bytesToRead -= bytesRead;
        }

fastExit:
        
        /* Check link state, e.g. via MDIO communication with PHY */
        /* Cyclic lwIP timers check */
        printf("LWIP: CHECK TIMEOUTS\n");

        sys_check_timeouts();

        printf("LWIP: CHECK TIMEOUTS DONE\n");

        //struct dhcp* myaddr = netif_dhcp_data(&_netif);
        //printf("DHCP: state:[%X] server[%X] addr:[%X] gw:[%X] mask:[%X]\n",myaddr->state, myaddr->server_ip_addr.addr, myaddr->offered_gw_addr.addr, myaddr->offered_ip_addr.addr, myaddr->offered_sn_mask.addr);
            //w5500_dump_state();
        //usleep(5000);

    }
     

    return 0;
}

