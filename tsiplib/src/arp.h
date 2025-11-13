#ifndef ARP_H
#define ARP_H

#include "types.h"
#include "net.h"

/* ARP constants */
#define ARP_HW_TYPE_ETH     1
#define ARP_PROTO_TYPE_IP   0x0800
#define ARP_OP_REQUEST      1
#define ARP_OP_REPLY        2

/* ARP cache timeout (in seconds) */
#define ARP_CACHE_TIMEOUT   300

/* ARP header structure */
typedef struct {
    uint16_t hw_type;
    uint16_t proto_type;
    uint8_t  hw_size;
    uint8_t  proto_size;
    uint16_t opcode;
    eth_addr_t sender_mac;
    ip_addr_t  sender_ip;
    eth_addr_t target_mac;
    ip_addr_t  target_ip;
} arp_header_t;

/* ARP functions */
bool arp_init(void);
bool arp_resolve(const ip_addr_t *ip, eth_addr_t *mac);
void arp_announce(const ip_addr_t *ip);
void arp_cache_add(const ip_addr_t *ip, const eth_addr_t *mac);
void arp_cache_flush(void);

#endif /* ARP_H */