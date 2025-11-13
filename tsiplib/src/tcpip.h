#ifndef TCPIP_H
#define TCPIP_H

/* Main header file for the TCP/IP stack */

#include "types.h"
#include "net.h"
#include "hal.h"
#include "ethernet.h"
#include "arp.h"
#include "ip.h"
#include "icmp.h"
#include "udp.h"
#include "dhcp.h"

/* Stack initialization and management */
bool tcpip_init(net_driver_t *driver);
void tcpip_poll(void);
void tcpip_shutdown(void);

/* Network configuration */
bool tcpip_set_ip_config(const ip_addr_t *ip, const ip_addr_t *netmask, const ip_addr_t *gateway);
bool tcpip_get_ip_config(ip_addr_t *ip, ip_addr_t *netmask, ip_addr_t *gateway);

/* Utility functions */
void tcpip_get_mac_address(eth_addr_t *mac);
bool tcpip_is_link_up(void);

/* String conversion utilities */
bool ip_addr_from_str(const char *str, ip_addr_t *addr);
void ip_addr_to_str(const ip_addr_t *addr, char *str);
void mac_addr_to_str(const eth_addr_t *addr, char *str);

#endif /* TCPIP_H */