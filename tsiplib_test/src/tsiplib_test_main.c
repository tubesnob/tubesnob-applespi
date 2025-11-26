/* Example TCP/IP stack application for 65816 */
#include <stdio.h>
#include <string.h>
#include "tsiplib_test_main.h"

#ifdef __MACOS__
#include "../../tsspilib_driver_ftdi/src/tsspilib_driver_ftdi.h"
#endif

#ifdef __APPLE2GS__
#include "../../tsspilib_driver_a2gpio/src/tsspilib_driver_a2gpio.h"
#endif

uint8_t _hwaddr[]   = { 0x80, 0x70, 0x60, 0x50, 0x40, 0x30 };
uint8_t _ipaddr[]   = { 10, 0, 0, 161 };
uint8_t _mask[]     = { 255, 255, 255, 0 };
uint8_t _gwaddr[]   = { 10, 0, 0, 1};

static tsiplib_config_t _ip_config;

/* DHCP state callback */
static void dhcp_state_changed(dhcp_state_t state)
{
    printf("DHCP state: ");
    switch (state) {
        case DHCP_STATE_INIT:
            printf("INIT\n");
            break;
        case DHCP_STATE_SELECTING:
            printf("SELECTING\n");
            break;
        case DHCP_STATE_REQUESTING:
            printf("REQUESTING\n");
            break;
        case DHCP_STATE_BOUND:
            printf("BOUND - Got IP address!\n");
            break;
        default:
            printf("Unknown\n");
    }
}

/* ICMP echo reply callback */
static void ping_reply(const ip_addr_t *src, uint16_t id, uint16_t seq, 
                      const uint8_t *data, uint16_t len)
{
    char ip_str[16];
    ip_addr_to_str(src, ip_str);
    printf("Ping reply from %s: seq=%d, len=%d\n", ip_str, seq, len);
}

/* UDP echo server callback */
static void udp_echo_rx(const uint8_t *data, uint16_t len, 
                       const ip_addr_t *src, uint16_t src_port)
{
    char ip_str[16];
    ip_addr_to_str(src, ip_str);
    printf("UDP echo from %s:%d - %d bytes\n", ip_str, src_port, len);
    /* Echo server would send the data back here */
}

int main(int argc, char** argv)
{
    ip_addr_t src_ip; 
    ip_addr_t src_netmask;
    ip_addr_t src_gateway;
    ip_addr_t src_mac;

    udp_socket_t *echo_socket;

    printf("TCP/IP Stack Example for 65816\n");
    printf("==============================\n\n");

    printf("Initializing W5500 driver\r\n");
    tsiplib_w5500_driver_init();
    
    printf("Creating W5500 Driver VTBL\r\n");
    net_driver_t* w5500_driver = tsiplib_w5500_driver_create();

    printf("Setting Ethernet MAC Address\r\n");
    w5500_driver->set_mac_addr(_hwaddr);

    _ip_config.enable_arp = 1;
    _ip_config.enable_dhcp = 0;
    _ip_config.enable_ethernet = 1;
    _ip_config.enable_icmp = 1;
    _ip_config.enable_ip = 1;
    _ip_config.enable_udp = 1;

    printf("Initializing TCP/IP Stack\r\n");
    if (!tcpip_init(w5500_driver, &_ip_config)) {
        printf("Failed to initialize TCP/IP stack\n");
        return 1;
    }

    printf("Setting SRCIP, MASK, GW\r\n");
    tcpip_set_ip_config(_ipaddr, _mask, _gwaddr);

    printf("Retrieving SRC IP, MASK, GW\r\n");
    w5500_driver->get_mac_addr(&src_mac);
    tcpip_get_ip_config(&src_ip, &src_netmask, &src_gateway);


    w5500_driver->dump_status();

    char ip_str[16];
    char mac_str[18];

    mac_addr_to_str(&src_mac, mac_str);
    printf("MAC Address: %s\n", mac_str);
    ip_addr_to_str(&src_ip, ip_str);
    printf("IP Address: %s\n", ip_str);
    ip_addr_to_str(&src_netmask, ip_str);
    printf("Netmask: %s\n", ip_str);
    ip_addr_to_str(&src_gateway, ip_str);
    printf("GW Address: %s\n", ip_str);

    /*

    dhcp_config_t dhcp_cfg;
    int use_dhcp = 0;  

    if (use_dhcp) {
        printf("\nStarting DHCP...\n");
        dhcp_set_callback(dhcp_state_changed);
        
        if (!dhcp_start()) {
            printf("Failed to start DHCP\n");
            return 1;
        }
        
        while (dhcp_get_state() != DHCP_STATE_BOUND) {
            tcpip_poll();
        }
        
        if (dhcp_get_config(&dhcp_cfg)) {
            ip_addr_to_str(&dhcp_cfg.ip, ip_str);
            printf("\nIP Address: %s\n", ip_str);
            
            ip_addr_to_str(&dhcp_cfg.netmask, ip_str);
            printf("Netmask: %s\n", ip_str);
            
            ip_addr_to_str(&dhcp_cfg.gateway, ip_str);
            printf("Gateway: %s\n", ip_str);
            
            ip_addr_to_str(&dhcp_cfg.dns, ip_str);
            printf("DNS Server: %s\n", ip_str);
            
            printf("Lease Time: %lu seconds\n", dhcp_cfg.lease_time);
        }
    } else {
        ip_addr_from_str("192.168.1.100", &my_ip);
        ip_addr_from_str("255.255.255.0", &netmask);
        ip_addr_from_str("192.168.1.1", &gateway);
        
        if (!tcpip_set_ip_config(&my_ip, &netmask, &gateway)) {
            printf("Failed to set IP configuration\n");
            return 1;
        }
        
        printf("\nStatic IP configuration:\n");
        printf("IP Address: 192.168.1.100\n");
        printf("Netmask: 255.255.255.0\n");
        printf("Gateway: 192.168.1.1\n");
    }
    */
    
    /* Set up ICMP echo reply handler */
    icmp_set_echo_callback(ping_reply);
    
    /* Create UDP echo server on port 7 */
    echo_socket = udp_socket_create();
    if (echo_socket) {
        if (udp_socket_bind(echo_socket, 7)) {
            udp_socket_set_rx_callback(echo_socket, udp_echo_rx);
            printf("\nUDP echo server listening on port 7\n");
        }
    }
    
    
    printf("\nMain loop running. Press Ctrl+C to exit.\n");
    printf("The stack will:\n");
    printf("- Respond to ARP requests\n");
    printf("- Reply to ping (ICMP echo) requests\n");
    printf("- Echo UDP packets sent to port 7\n\n");

    int hasSentPing = 0;
    while (1) {
        if (!hasSentPing) {
            printf("\nSending ping to gateway...\n");
            tcpip_get_ip_config(&src_ip, &src_netmask, &src_gateway);
            icmp_send_echo_request(&src_gateway, 1, 1, (uint8_t *)"Hello", 5);
            hasSentPing = 1;
        }
        tcpip_poll();
    }
    
    return 0;
}

