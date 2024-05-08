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

#ifdef __MACOS__
#include "../../tsspilib_driver_ftdi/src/tsspilib_driver_ftdi.h"
#endif

#ifdef __APPLE2GS__
#include "../../tsspilib_driver_a2gpio/src/tsspilib_driver_a2gpio.h"
#endif

uint8_t _hwaddr[]   = { 0x80, 0x70, 0x60, 0x50, 0x40, 0x30 };
uint8_t _ipaddr[]   = { 10, 0, 0, 133 };
uint8_t _mask[]     = { 255, 255, 255, 0 };
uint8_t _gwaddr[]   = { 10, 0, 0, 1};

struct ip_globals ip_data;
struct netif _netif;

socket_t* _socket;
unsigned char _mac_send_buffer[102048];
unsigned char _mac_read_buffer[102048];

u32_t lwip_port_rand(void)
{
  return (u32_t)rand();
}

u32_t sys_now() {
    return clock();
}

tsspilib_device_vtbl_t* _spiDevice = NULL;

int initializeSPI();
int initializeW5500();
int initializeSockets();
int initializeLWIP();

err_t ts_netif_init(struct netif *netif);
err_t ts_netif_output(struct netif *netif, struct pbuf *p);

int main(int argc, char** argv)
{
    tslib_init();

    int counter;
    int counter2;
    counter = 0;

    initializeSPI();
    initializeW5500();
    initializeSockets();
    initializeLWIP();
    /*
    _tslog->info("Creating Socket\n");
    socket_t* socket = socket_create(0x36, 8044);

    waitMilliseconds(1000);
    uint8_t rbuf[] = { 0,0,0,0,0,0 };

    _tslog->info("Creating Socket\n");
    socket_t* socket = socket_create(0x36, 8044);

    waitMilliseconds(1000);
    */


  

}

err_t ts_netif_output(struct netif *netif, struct pbuf *p)
{
  /* Update SNMP stats (only if you use SNMP) */
  pbuf_copy_partial(p, _mac_send_buffer, p->tot_len, 0);
  _socket->send(_socket, _mac_send_buffer, p->tot_len);
  /* Start MAC transmit here */
  return ERR_OK;
}

void ts_netif_status_callback(struct netif *netif)
{
  printf("netif status changed %s\n", ip4addr_ntoa(netif_ip4_addr(netif)));
}

err_t ts_netif_init(struct netif *netif)
{
    netif->linkoutput = ts_netif_output;
    netif->output     = etharp_output; 
    //netif->output_ip6 = NULL; // NEED FILL IN ethip6_output;
    netif->mtu        = 1508; // DEFAULT VALUE? ETHERNET_MTU;
    netif->flags      = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET | NETIF_FLAG_IGMP | NETIF_FLAG_MLD6;
    MIB2_INIT_NETIF(netif, snmp_ifType_ethernet_csmacd, 100000000);
    return ERR_OK;
}

int intmin(int x, int y) {
  return (x < y) ? x : y;
}

int initializeLWIP() {
  
    lwip_init();
pbuf_init();

    netif_add(&_netif, IP4_ADDR_ANY, IP4_ADDR_ANY, IP4_ADDR_ANY, NULL, ts_netif_init, netif_input);

    _netif.name[0] = 'e';
    _netif.name[1] = '0';

    SMEMCPY(_netif.hwaddr, _hwaddr, 6);
    _netif.hwaddr_len = 6;

    netif_set_status_callback(&_netif, ts_netif_status_callback);
    netif_set_default(&_netif);
    netif_set_up(&_netif);
    netif_set_link_up(&_netif);
    
    /* Start DHCP and HTTPD */
    err_t x = dhcp_start(&_netif );

    //struct pbuf* buffer = pbuf_alloc(PBUF_RAW, 2048, PBUF_POOL);
    //pbuf_ref(buffer);
    while(1) {


        _socket->refresh(_socket);

        printf("Avail:%i\n",_socket->rx_bytes_available);

        int bytesToRead = _socket->rx_bytes_available;
        while (bytesToRead) {
            int chunkSize = intmin(bytesToRead, 1600);
            _socket->receive(_socket, _mac_read_buffer, chunkSize);
            struct pbuf* buffer = pbuf_alloc(PBUF_RAW, chunkSize, PBUF_POOL);
            pbuf_take(buffer, _mac_read_buffer, chunkSize);
            if(_netif.input(buffer, &_netif) != ERR_OK) {
                printf("asd\n");
            }
            bytesToRead -= chunkSize;
        }
        

        
        /* Check link state, e.g. via MDIO communication with PHY */
        /* Cyclic lwIP timers check */
        sys_check_timeouts();

        struct dhcp* myaddr = netif_dhcp_data(&_netif);
        printf("DHCP: state:[%X] server[%X] addr:[%X] gw:[%X] mask:[%X]\n",myaddr->state, myaddr->server_ip_addr.addr, myaddr->offered_gw_addr.addr, myaddr->offered_ip_addr.addr, myaddr->offered_sn_mask.addr);
            //w5500_dump_state();
        usleep(5000);

    }
     

    return 0;
}


int initializeSPI() {
    _tslog->info("Initializing SPI\n");
    #ifdef __APPLE2GS__
    _spiDevice = a2gpio_spi_driver_load();
    #else
    _spiDevice = ftdi_spi_driver_load();
    #endif
    if (_spiDevice==NULL) {
        printf("No device loaded\n");
        return SPI_ERROR;
    }
    int rv = spi_init(_spiDevice);
    _tslog->info("SPI is initialized\n");
    return rv;
}

int initializeW5500() {
   
    _tslog->info("Initializing W5500\n");
    w5500_init();

    _tslog->info("Resetting W5500\n");
    w5500_reset();

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

int initializeSockets() {

    socket_init();

    _socket = socket_create_raw();

    return 0;
}



  