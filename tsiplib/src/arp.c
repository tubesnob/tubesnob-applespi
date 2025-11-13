#include "arp.h"
#include "ethernet.h"
#include "hal.h"
#include <string.h>

/* ARP cache entry */
typedef struct {
    ip_addr_t ip;
    eth_addr_t mac;
    uint16_t timeout;  /* Simple counter, decremented periodically */
    bool valid;
} arp_entry_t;

/* ARP cache */
#define ARP_CACHE_SIZE 16
static arp_entry_t arp_cache[ARP_CACHE_SIZE];

/* Our IP address (will be set by DHCP or manually) */
static ip_addr_t our_ip = {{0, 0, 0, 0}};

/* Forward declarations */
static void arp_rx_callback(const uint8_t *data, uint16_t len, const eth_addr_t *src);
static bool arp_send(uint16_t opcode, const eth_addr_t *target_mac, const ip_addr_t *target_ip);

bool arp_init(void)
{
    /* Clear ARP cache */
    memset(arp_cache, 0, sizeof(arp_cache));
    
    /* Register for ARP packets */
    eth_set_rx_callback(ETH_TYPE_ARP, arp_rx_callback);
    
    return true;
}

bool arp_resolve(const ip_addr_t *ip, eth_addr_t *mac)
{
    uint8_t i;
    uint8_t attempts;
    
    if (!ip || !mac) {
        return false;
    }
    
    /* Check cache first */
    for (i = 0; i < ARP_CACHE_SIZE; i++) {
        if (arp_cache[i].valid && 
            memcmp(&arp_cache[i].ip, ip, sizeof(ip_addr_t)) == 0) {
            eth_addr_copy(mac, &arp_cache[i].mac);
            return true;
        }
    }
    
    /* Not in cache, send ARP request */
    for (attempts = 0; attempts < 3; attempts++) {
        if (!arp_send(ARP_OP_REQUEST, &eth_broadcast, ip)) {
            return false;
        }
        
        /* Poll for response (simple busy wait) */
        for (i = 0; i < 100; i++) {
            eth_poll();
            
            /* Check if address was added to cache */
            uint8_t j;
            for (j = 0; j < ARP_CACHE_SIZE; j++) {
                if (arp_cache[j].valid && 
                    memcmp(&arp_cache[j].ip, ip, sizeof(ip_addr_t)) == 0) {
                    eth_addr_copy(mac, &arp_cache[j].mac);
                    return true;
                }
            }
        }
    }
    
    return false;
}

void arp_announce(const ip_addr_t *ip)
{
    if (!ip) {
        return;
    }
    
    /* Update our IP */
    memcpy(&our_ip, ip, sizeof(ip_addr_t));
    
    /* Send gratuitous ARP */
    arp_send(ARP_OP_REQUEST, &eth_broadcast, ip);
}

void arp_cache_add(const ip_addr_t *ip, const eth_addr_t *mac)
{
    uint8_t i;
    uint8_t oldest = 0;
    uint16_t oldest_time = 0xFFFF;
    
    if (!ip || !mac) {
        return;
    }
    
    /* Check if already in cache */
    for (i = 0; i < ARP_CACHE_SIZE; i++) {
        if (arp_cache[i].valid && 
            memcmp(&arp_cache[i].ip, ip, sizeof(ip_addr_t)) == 0) {
            /* Update existing entry */
            eth_addr_copy(&arp_cache[i].mac, mac);
            arp_cache[i].timeout = ARP_CACHE_TIMEOUT;
            return;
        }
    }
    
    /* Find free slot or oldest entry */
    for (i = 0; i < ARP_CACHE_SIZE; i++) {
        if (!arp_cache[i].valid) {
            /* Use free slot */
            memcpy(&arp_cache[i].ip, ip, sizeof(ip_addr_t));
            eth_addr_copy(&arp_cache[i].mac, mac);
            arp_cache[i].timeout = ARP_CACHE_TIMEOUT;
            arp_cache[i].valid = true;
            return;
        }
        if (arp_cache[i].timeout < oldest_time) {
            oldest = i;
            oldest_time = arp_cache[i].timeout;
        }
    }
    
    /* Replace oldest entry */
    memcpy(&arp_cache[oldest].ip, ip, sizeof(ip_addr_t));
    eth_addr_copy(&arp_cache[oldest].mac, mac);
    arp_cache[oldest].timeout = ARP_CACHE_TIMEOUT;
    arp_cache[oldest].valid = true;
}

void arp_cache_flush(void)
{
    memset(arp_cache, 0, sizeof(arp_cache));
}

static void arp_rx_callback(const uint8_t *data, uint16_t len, const eth_addr_t *src)
{
    arp_header_t *arp;
    eth_addr_t our_mac;
    
    if (len < sizeof(arp_header_t)) {
        return;
    }
    
    arp = (arp_header_t *)data;
    
    /* Check ARP packet validity */
    if (ntohs(arp->hw_type) != ARP_HW_TYPE_ETH ||
        ntohs(arp->proto_type) != ARP_PROTO_TYPE_IP ||
        arp->hw_size != ETH_ADDR_LEN ||
        arp->proto_size != 4) {
        return;
    }
    
    /* Add sender to cache */
    arp_cache_add(&arp->sender_ip, &arp->sender_mac);
    
    /* Check if packet is for us */
    if (memcmp(&arp->target_ip, &our_ip, sizeof(ip_addr_t)) != 0) {
        return;
    }
    
    /* Handle ARP request */
    if (ntohs(arp->opcode) == ARP_OP_REQUEST) {
        /* Send ARP reply */
        arp_send(ARP_OP_REPLY, &arp->sender_mac, &arp->sender_ip);
    }
}

static bool arp_send(uint16_t opcode, const eth_addr_t *target_mac, const ip_addr_t *target_ip)
{
    arp_header_t arp;
    eth_addr_t our_mac;
    
    /* Get our MAC address */
    hal_get_mac_addr(&our_mac);
    
    /* Build ARP packet */
    arp.hw_type = htons(ARP_HW_TYPE_ETH);
    arp.proto_type = htons(ARP_PROTO_TYPE_IP);
    arp.hw_size = ETH_ADDR_LEN;
    arp.proto_size = 4;
    arp.opcode = htons(opcode);
    
    eth_addr_copy(&arp.sender_mac, &our_mac);
    memcpy(&arp.sender_ip, &our_ip, sizeof(ip_addr_t));
    
    if (opcode == ARP_OP_REQUEST) {
        /* For requests, target MAC is unknown */
        memset(&arp.target_mac, 0, ETH_ADDR_LEN);
    } else {
        eth_addr_copy(&arp.target_mac, target_mac);
    }
    memcpy(&arp.target_ip, target_ip, sizeof(ip_addr_t));
    
    /* Send ARP packet */
    return eth_send(target_mac, ETH_TYPE_ARP, (uint8_t *)&arp, sizeof(arp));
}