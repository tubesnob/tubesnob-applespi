#ifndef NET_H
#define NET_H

#include "types.h"

/* Ethernet constants */
#define ETH_ADDR_LEN        6
#define ETH_HEADER_LEN      14
#define ETH_MIN_FRAME_LEN   60
#define ETH_MAX_FRAME_LEN   1514
#define ETH_MTU             1500

/* Ethernet frame types */
#define ETH_TYPE_IP         0x0800
#define ETH_TYPE_ARP        0x0806

/* IP protocol numbers */
#define IP_PROTO_ICMP       1
#define IP_PROTO_TCP        6
#define IP_PROTO_UDP        17

/* Common port numbers */
#define DHCP_CLIENT_PORT    68
#define DHCP_SERVER_PORT    67

/* MAC address structure */
typedef struct {
    uint8_t addr[ETH_ADDR_LEN];
} eth_addr_t;

/* IP address structure */
typedef struct {
    uint8_t addr[4];
} ip_addr_t;

/* Ethernet header */
typedef struct {
    eth_addr_t dest;
    eth_addr_t src;
    uint16_t type;
} eth_header_t;

/* IP header */
typedef struct {
    uint8_t  version_ihl;
    uint8_t  tos;
    uint16_t total_length;
    uint16_t identification;
    uint16_t flags_fragment;
    uint8_t  ttl;
    uint8_t  protocol;
    uint16_t checksum;
    ip_addr_t src;
    ip_addr_t dest;
} ip_header_t;

/* UDP header */
typedef struct {
    uint16_t src_port;
    uint16_t dest_port;
    uint16_t length;
    uint16_t checksum;
} udp_header_t;

/* Network buffer structure */
typedef struct {
    uint8_t *data;
    uint16_t size;
    uint16_t len;
} netbuf_t;

#endif /* NET_H */