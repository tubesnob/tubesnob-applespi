#include "tsiplib.h"
#include <string.h>
#include <stdio.h>

extern net_driver_t* _driver;

/* Broadcast MAC address */
const eth_addr_t eth_broadcast = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};

/* Our MAC address */
static eth_addr_t our_mac;

/* Receive callbacks */
typedef struct {
    uint16_t type;
    void (*callback)(const uint8_t *data, uint16_t len, const eth_addr_t *src);
} eth_callback_t;

#define MAX_CALLBACKS 8
static eth_callback_t callbacks[MAX_CALLBACKS];
static uint8_t num_callbacks = 0;

/* Receive buffer */
static uint8_t rx_buffer[ETH_MAX_FRAME_LEN];

bool eth_init(void)
{

    _driver->get_mac_addr(&our_mac);
    
    /* Clear callbacks */
    num_callbacks = 0;
    memset(callbacks, 0, sizeof(callbacks));
    
    return true;
}

bool eth_send(const eth_addr_t *dest, uint16_t type, const uint8_t *data, uint16_t len)
{
    uint8_t frame[ETH_MAX_FRAME_LEN];
    eth_header_t *hdr;
    uint16_t frame_len;
    
    /* Check parameters */
    if (!dest || !data || len > ETH_MTU) {
        return false;
    }
    
    /* Build Ethernet header */
    hdr = (eth_header_t *)frame;
    eth_addr_copy(&hdr->dest, dest);
    eth_addr_copy(&hdr->src, &our_mac);
    hdr->type = htons(type);
    
    /* Copy payload */
    memcpy(frame + ETH_HEADER_LEN, data, len);
    frame_len = ETH_HEADER_LEN + len;
    
    /* Pad to minimum frame size if needed */
    if (frame_len < ETH_MIN_FRAME_LEN) {
        memset(frame + frame_len, 0, ETH_MIN_FRAME_LEN - frame_len);
        frame_len = ETH_MIN_FRAME_LEN;
    }
    
    _driver->send_packet(frame, frame_len);
}

void eth_poll(void)
{
    uint16_t len;
    eth_header_t *hdr;
    uint16_t type;
    uint8_t i;
    
    /* Receive packet */
    len = _driver->recv_packet(rx_buffer, sizeof(rx_buffer));
    if (len < ETH_HEADER_LEN) {
        return;
    }
    
    hdr = (eth_header_t *)rx_buffer;
    
    /* Check if packet is for us */
    if (!eth_addr_is_equal(&hdr->dest, &our_mac) && 
        !eth_addr_is_broadcast(&hdr->dest) &&
        !eth_addr_is_multicast(&hdr->dest)) {
        return;
    }
    
    /* Get frame type */
    type = ntohs(hdr->type);
    
    /* Call registered callbacks */
    for (i = 0; i < num_callbacks; i++) {
        if (callbacks[i].type == type && callbacks[i].callback) {
            callbacks[i].callback(rx_buffer + ETH_HEADER_LEN, 
                                len - ETH_HEADER_LEN, 
                                &hdr->src);
        }
    }
}

void eth_set_rx_callback(uint16_t type, void (*callback)(const uint8_t *data, uint16_t len, const eth_addr_t *src))
{
    uint8_t i;
    
    if (!callback || num_callbacks >= MAX_CALLBACKS) {
        return;
    }
    
    /* Check if callback already exists */
    for (i = 0; i < num_callbacks; i++) {
        if (callbacks[i].type == type) {
            callbacks[i].callback = callback;
            return;
        }
    }
    
    /* Add new callback */
    callbacks[num_callbacks].type = type;
    callbacks[num_callbacks].callback = callback;
    num_callbacks++;
}

bool eth_addr_is_equal(const eth_addr_t *a, const eth_addr_t *b)
{
    return memcmp(a->addr, b->addr, ETH_ADDR_LEN) == 0;
}

void eth_addr_copy(eth_addr_t *dest, const eth_addr_t *src)
{
    memcpy(dest->addr, src->addr, ETH_ADDR_LEN);
}

bool eth_addr_is_multicast(const eth_addr_t *addr)
{
    return (addr->addr[0] & 0x01) != 0;
}

bool eth_addr_is_broadcast(const eth_addr_t *addr)
{
    return eth_addr_is_equal(addr, &eth_broadcast);
}

void eth_print_frame(const uint8_t *frame, uint16_t len)
{
    eth_header_t *hdr;
    uint16_t type;
    char src_str[32], dest_str[32];
    
    /* Validate frame */
    if (!frame || len < ETH_HEADER_LEN) {
        printf("Invalid frame: too short (%d bytes)\n", len);
        return;
    }
    
    hdr = (eth_header_t *)frame;
    type = ntohs(hdr->type);
    
    /* Convert addresses to strings */
    mac_addr_to_str(&hdr->src, src_str);
    mac_addr_to_str(&hdr->dest, dest_str);
    
    /* Print frame header */
    printf("========== Ethernet Frame ==========\n");
    printf("Frame length: %d bytes\n", len);
    printf("Source MAC:   %s\n", src_str);
    printf("Dest MAC:     %s", dest_str);
    
    /* Add descriptive info for special addresses */
    if (eth_addr_is_broadcast(&hdr->dest)) {
        printf(" (Broadcast)");
    } else if (eth_addr_is_multicast(&hdr->dest)) {
        printf(" (Multicast)");
    }
    printf("\n");
    
    /* Print frame type */
    printf("Type:         0x%04X", type);
    switch (type) {
        case ETH_TYPE_IP:
            printf(" (IPv4)");
            break;
        case ETH_TYPE_ARP:
            printf(" (ARP)");
            break;
        default:
            printf(" (Unknown)");
            break;
    }
    printf("\n");
    
    /* If it's an IP packet, print IP header info */
    if (type == ETH_TYPE_IP && len >= ETH_HEADER_LEN + sizeof(ip_header_t)) {
        ip_header_t *ip_hdr = (ip_header_t *)(frame + ETH_HEADER_LEN);
        char src_ip[16], dest_ip[16];
        uint8_t version = (ip_hdr->version_ihl >> 4) & 0x0F;
        uint8_t ihl = ip_hdr->version_ihl & 0x0F;
        uint16_t total_len = ntohs(ip_hdr->total_length);
        
        ip_addr_to_str(&ip_hdr->src, src_ip);
        ip_addr_to_str(&ip_hdr->dest, dest_ip);
        
        printf("\n--- IP Header ---\n");
        printf("Version:      %d\n", version);
        printf("Header len:   %d bytes\n", ihl * 4);
        printf("Total len:    %d bytes\n", total_len);
        printf("Protocol:     %d", ip_hdr->protocol);
        
        switch (ip_hdr->protocol) {
            case IP_PROTO_ICMP:
                printf(" (ICMP)");
                break;
            case IP_PROTO_TCP:
                printf(" (TCP)");
                break;
            case IP_PROTO_UDP:
                printf(" (UDP)");
                break;
        }
        printf("\n");
        
        printf("Source IP:    %s\n", src_ip);
        printf("Dest IP:      %s\n", dest_ip);
        printf("TTL:          %d\n", ip_hdr->ttl);
        
        /* If it's UDP, print UDP header info */
        if (ip_hdr->protocol == IP_PROTO_UDP && 
            len >= ETH_HEADER_LEN + (ihl * 4) + sizeof(udp_header_t)) {
            udp_header_t *udp_hdr = (udp_header_t *)(frame + ETH_HEADER_LEN + (ihl * 4));
            
            printf("\n--- UDP Header ---\n");
            printf("Source port:  %d\n", ntohs(udp_hdr->src_port));
            printf("Dest port:    %d\n", ntohs(udp_hdr->dest_port));
            printf("Length:       %d bytes\n", ntohs(udp_hdr->length));
        }
    }
    /* If it's an ARP packet, print ARP info */
    else if (type == ETH_TYPE_ARP && len >= ETH_HEADER_LEN + sizeof(arp_header_t)) {
        arp_header_t *arp_hdr = (arp_header_t *)(frame + ETH_HEADER_LEN);
        char sender_mac[32], target_mac[32];
        char sender_ip[16], target_ip[16];
        uint16_t opcode = ntohs(arp_hdr->opcode);
        
        mac_addr_to_str(&arp_hdr->sender_mac, sender_mac);
        mac_addr_to_str(&arp_hdr->target_mac, target_mac);
        ip_addr_to_str(&arp_hdr->sender_ip, sender_ip);
        ip_addr_to_str(&arp_hdr->target_ip, target_ip);
        
        printf("\n--- ARP Header ---\n");
        printf("Operation:    %d", opcode);
        if (opcode == ARP_OP_REQUEST) {
            printf(" (Request)");
        } else if (opcode == ARP_OP_REPLY) {
            printf(" (Reply)");
        }
        printf("\n");
        printf("Sender MAC:   %s\n", sender_mac);
        printf("Sender IP:    %s\n", sender_ip);
        printf("Target MAC:   %s\n", target_mac);
        printf("Target IP:    %s\n", target_ip);
    }
    
    printf("====================================\n");
}