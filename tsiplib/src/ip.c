#include "ip.h"
#include "ethernet.h"
#include "arp.h"
#include <string.h>

/* IP configuration */
static ip_addr_t our_ip = {{0, 0, 0, 0}};
static ip_addr_t netmask = {{255, 255, 255, 0}};
static ip_addr_t gateway = {{0, 0, 0, 0}};

/* Protocol handlers */
typedef struct {
    uint8_t protocol;
    ip_protocol_handler_t handler;
} protocol_handler_t;

#define MAX_PROTOCOLS 8
static protocol_handler_t protocol_handlers[MAX_PROTOCOLS];
static uint8_t num_handlers = 0;

/* Packet ID counter */
static uint16_t packet_id = 0;

/* Forward declarations */
static void ip_rx_callback(const uint8_t *data, uint16_t len, const eth_addr_t *src);

bool ip_init(void)
{
    /* Clear protocol handlers */
    num_handlers = 0;
    memset(protocol_handlers, 0, sizeof(protocol_handlers));
    
    /* Register for IP packets */
    eth_set_rx_callback(ETH_TYPE_IP, ip_rx_callback);
    
    return true;
}

bool ip_send(const ip_addr_t *dest, uint8_t protocol, const uint8_t *data, uint16_t len)
{
    uint8_t packet[ETH_MTU];
    ip_header_t *ip;
    eth_addr_t dest_mac;
    ip_addr_t next_hop;
    uint16_t total_len;
    
    if (!dest || !data || len > (ETH_MTU - sizeof(ip_header_t))) {
        return false;
    }
    
    /* Build IP header */
    ip = (ip_header_t *)packet;
    ip->version_ihl = (IP_VERSION << 4) | IP_IHL_MIN;
    ip->tos = 0;
    total_len = sizeof(ip_header_t) + len;
    ip->total_length = htons(total_len);
    ip->identification = htons(packet_id++);
    ip->flags_fragment = htons(IP_FLAG_DF);  /* Don't fragment */
    ip->ttl = IP_TTL_DEFAULT;
    ip->protocol = protocol;
    ip->checksum = 0;  /* Will calculate below */
    ip_addr_copy(&ip->src, &our_ip);
    ip_addr_copy(&ip->dest, dest);
    
    /* Calculate header checksum */
    ip->checksum = ip_checksum((uint8_t *)ip, sizeof(ip_header_t));
    
    /* Copy payload */
    memcpy(packet + sizeof(ip_header_t), data, len);
    
    /* Determine next hop */
    if (ip_addr_is_local(dest)) {
        /* Same subnet, send directly */
        ip_addr_copy(&next_hop, dest);
    } else {
        /* Different subnet, send to gateway */
        ip_addr_copy(&next_hop, &gateway);
    }
    
    /* Resolve MAC address */
    if (!arp_resolve(&next_hop, &dest_mac)) {
        return false;
    }
    
    /* Send packet */
    return eth_send(&dest_mac, ETH_TYPE_IP, packet, total_len);
}

void ip_set_address(const ip_addr_t *addr)
{
    if (addr) {
        ip_addr_copy(&our_ip, addr);
        /* Announce new IP via ARP */
        arp_announce(&our_ip);
    }
}

void ip_get_address(ip_addr_t *addr)
{
    if (addr) {
        ip_addr_copy(addr, &our_ip);
    }
}

void ip_set_netmask(const ip_addr_t *mask)
{
    if (mask) {
        ip_addr_copy(&netmask, mask);
    }
}

void ip_get_netmask(ip_addr_t *mask)
{
    if (mask) {
        ip_addr_copy(mask, &netmask);
    }
}

void ip_set_gateway(const ip_addr_t *gw)
{
    if (gw) {
        ip_addr_copy(&gateway, gw);
    }
}

void ip_get_gateway(ip_addr_t *gw)
{
    if (gw) {
        ip_addr_copy(gw, &gateway);
    }
}

uint16_t ip_checksum(const uint8_t *data, uint16_t len)
{
    uint32_t sum = 0;
    const uint16_t *ptr = (const uint16_t *)data;
    
    /* Sum all 16-bit words */
    while (len > 1) {
        sum += *ptr++;
        len -= 2;
    }
    
    /* Add left-over byte, if any */
    if (len > 0) {
        sum += *(const uint8_t *)ptr;
    }
    
    /* Add carries */
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }
    
    /* One's complement */
    return ~sum;
}

bool ip_addr_is_equal(const ip_addr_t *a, const ip_addr_t *b)
{
    return memcmp(a->addr, b->addr, 4) == 0;
}

void ip_addr_copy(ip_addr_t *dest, const ip_addr_t *src)
{
    memcpy(dest->addr, src->addr, 4);
}

bool ip_addr_is_broadcast(const ip_addr_t *addr, const ip_addr_t *mask)
{
    uint8_t i;
    for (i = 0; i < 4; i++) {
        if ((addr->addr[i] & ~mask->addr[i]) != ~mask->addr[i]) {
            return false;
        }
    }
    return true;
}

bool ip_addr_is_multicast(const ip_addr_t *addr)
{
    return (addr->addr[0] & 0xF0) == 0xE0;
}

bool ip_addr_is_local(const ip_addr_t *addr)
{
    uint8_t i;
    for (i = 0; i < 4; i++) {
        if ((addr->addr[i] & netmask.addr[i]) != 
            (our_ip.addr[i] & netmask.addr[i])) {
            return false;
        }
    }
    return true;
}

void ip_register_protocol(uint8_t protocol, ip_protocol_handler_t handler)
{
    uint8_t i;
    
    if (!handler || num_handlers >= MAX_PROTOCOLS) {
        return;
    }
    
    /* Check if already registered */
    for (i = 0; i < num_handlers; i++) {
        if (protocol_handlers[i].protocol == protocol) {
            protocol_handlers[i].handler = handler;
            return;
        }
    }
    
    /* Add new handler */
    protocol_handlers[num_handlers].protocol = protocol;
    protocol_handlers[num_handlers].handler = handler;
    num_handlers++;
}

static void ip_rx_callback(const uint8_t *data, uint16_t len, const eth_addr_t *src)
{
    ip_header_t *ip;
    uint8_t ihl;
    uint16_t total_len;
    uint8_t i;
    
    if (len < sizeof(ip_header_t)) {
        return;
    }
    
    ip = (ip_header_t *)data;
    
    /* Check IP version */
    if ((ip->version_ihl >> 4) != IP_VERSION) {
        return;
    }
    
    /* Get header length */
    ihl = (ip->version_ihl & 0x0F) * 4;
    if (ihl < sizeof(ip_header_t) || ihl > len) {
        return;
    }
    
    /* Verify checksum */
    if (ip_checksum(data, ihl) != 0) {
        return;
    }
    
    /* Check total length */
    total_len = ntohs(ip->total_length);
    if (total_len > len) {
        return;
    }
    
    /* Check if packet is for us */
    if (!ip_addr_is_equal(&ip->dest, &our_ip) &&
        !ip_addr_is_broadcast(&ip->dest, &netmask) &&
        !ip_addr_is_multicast(&ip->dest)) {
        return;
    }
    
    /* Update ARP cache with sender */
    arp_cache_add(&ip->src, src);
    
    /* Call protocol handler */
    for (i = 0; i < num_handlers; i++) {
        if (protocol_handlers[i].protocol == ip->protocol && 
            protocol_handlers[i].handler) {
            protocol_handlers[i].handler(data + ihl, total_len - ihl, &ip->src);
            break;
        }
    }
}