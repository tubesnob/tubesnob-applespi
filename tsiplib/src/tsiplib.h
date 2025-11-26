#ifndef TSIPLIB_H
#define TSIPLIB_H

/* Combined header file for tsiplib TCP/IP stack */

/* System includes */
#include <stdint.h>
#include "../../orcadefaults.h"

/* Network byte order conversions */
#define htons(x) ((uint16_t)(((x) >> 8) | ((x) << 8)))
#define ntohs(x) htons(x)
#define htonl(x) ((uint32_t)(((x) >> 24) | (((x) >> 8) & 0xFF00) | \
                            (((x) << 8) & 0xFF0000) | ((x) << 24)))
#define ntohl(x) htonl(x)

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

/* === Ethernet layer (from ethernet.h) === */

/* Broadcast MAC address */
extern const eth_addr_t eth_broadcast;

/* Ethernet functions */
bool eth_init(void);
bool eth_send(const eth_addr_t *dest, uint16_t type, const uint8_t *data, uint16_t len);
void eth_poll(void);
void eth_set_rx_callback(uint16_t type, void (*callback)(const uint8_t *data, uint16_t len, const eth_addr_t *src));

/* Utility functions */
bool eth_addr_is_equal(const eth_addr_t *a, const eth_addr_t *b);
void eth_addr_copy(eth_addr_t *dest, const eth_addr_t *src);
bool eth_addr_is_multicast(const eth_addr_t *addr);
bool eth_addr_is_broadcast(const eth_addr_t *addr);

/* Debug function to print ethernet frame details */
void eth_print_frame(const uint8_t *frame, uint16_t len);

/* === ARP (from arp.h) === */

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

/* === IP layer (from ip.h) === */

/* IP version */
#define IP_VERSION 4

/* IP header fields */
#define IP_IHL_MIN 5
#define IP_IHL_MAX 15
#define IP_TTL_DEFAULT 64

/* IP flags */
#define IP_FLAG_DF 0x4000  /* Don't fragment */
#define IP_FLAG_MF 0x2000  /* More fragments */

/* IP functions */
bool ip_init(void);
bool ip_send(const ip_addr_t *dest, uint8_t protocol, const uint8_t *data, uint16_t len);
void ip_set_address(const ip_addr_t *addr);
void ip_get_address(ip_addr_t *addr);
void ip_set_netmask(const ip_addr_t *mask);
void ip_get_netmask(ip_addr_t *mask);
void ip_set_gateway(const ip_addr_t *gw);
void ip_get_gateway(ip_addr_t *gw);

/* IP utility functions */
uint16_t ip_checksum(const uint8_t *data, uint16_t len);
bool ip_addr_is_equal(const ip_addr_t *a, const ip_addr_t *b);
void ip_addr_copy(ip_addr_t *dest, const ip_addr_t *src);
bool ip_addr_is_broadcast(const ip_addr_t *addr, const ip_addr_t *mask);
bool ip_addr_is_multicast(const ip_addr_t *addr);
bool ip_addr_is_local(const ip_addr_t *addr);

/* Protocol handlers registration */
typedef void (*ip_protocol_handler_t)(const uint8_t *data, uint16_t len, const ip_addr_t *src);
void ip_register_protocol(uint8_t protocol, ip_protocol_handler_t handler);

/* === ICMP (from icmp.h) === */

/* ICMP types */
#define ICMP_TYPE_ECHO_REPLY    0
#define ICMP_TYPE_DEST_UNREACH  3
#define ICMP_TYPE_ECHO_REQUEST  8
#define ICMP_TYPE_TIME_EXCEEDED 11

/* ICMP codes for destination unreachable */
#define ICMP_CODE_NET_UNREACH   0
#define ICMP_CODE_HOST_UNREACH  1
#define ICMP_CODE_PROT_UNREACH  2
#define ICMP_CODE_PORT_UNREACH  3
#define ICMP_CODE_FRAG_NEEDED   4

/* ICMP header */
typedef struct {
    uint8_t  type;
    uint8_t  code;
    uint16_t checksum;
    uint16_t identifier;
    uint16_t sequence;
} icmp_header_t;

/* ICMP functions */
bool icmp_init(void);
bool icmp_send_echo_request(const ip_addr_t *dest, uint16_t id, uint16_t seq, const uint8_t *data, uint16_t len);
bool icmp_send_echo_reply(const ip_addr_t *dest, uint16_t id, uint16_t seq, const uint8_t *data, uint16_t len);

/* Callback for echo replies */
typedef void (*icmp_echo_callback_t)(const ip_addr_t *src, uint16_t id, uint16_t seq, const uint8_t *data, uint16_t len);
void icmp_set_echo_callback(icmp_echo_callback_t callback);

/* === UDP (from udp.h) === */

/* UDP socket structure */
typedef struct {
    uint16_t local_port;
    ip_addr_t remote_ip;
    uint16_t remote_port;
    void (*rx_callback)(const uint8_t *data, uint16_t len, const ip_addr_t *src, uint16_t src_port);
    bool in_use;
} udp_socket_t;

/* UDP functions */
bool udp_init(void);
udp_socket_t* udp_socket_create(void);
bool udp_socket_bind(udp_socket_t *socket, uint16_t port);
bool udp_socket_connect(udp_socket_t *socket, const ip_addr_t *remote_ip, uint16_t remote_port);
bool udp_send(udp_socket_t *socket, const uint8_t *data, uint16_t len);
bool udp_sendto(udp_socket_t *socket, const uint8_t *data, uint16_t len, const ip_addr_t *dest_ip, uint16_t dest_port);
void udp_socket_close(udp_socket_t *socket);

/* Set receive callback for socket */
void udp_socket_set_rx_callback(udp_socket_t *socket, 
    void (*callback)(const uint8_t *data, uint16_t len, const ip_addr_t *src, uint16_t src_port));

/* === DHCP (from dhcp.h) === */

/* DHCP states */
typedef enum {
    DHCP_STATE_INIT,
    DHCP_STATE_SELECTING,
    DHCP_STATE_REQUESTING,
    DHCP_STATE_BOUND,
    DHCP_STATE_RENEWING,
    DHCP_STATE_REBINDING
} dhcp_state_t;

/* DHCP configuration */
typedef struct {
    ip_addr_t ip;
    ip_addr_t netmask;
    ip_addr_t gateway;
    ip_addr_t dns;
    uint32_t lease_time;
} dhcp_config_t;

/* DHCP functions */
bool dhcp_init(void);
bool dhcp_start(void);
void dhcp_stop(void);
bool dhcp_poll(void);
dhcp_state_t dhcp_get_state(void);
bool dhcp_get_config(dhcp_config_t *config);

/* Callback for DHCP state changes */
typedef void (*dhcp_callback_t)(dhcp_state_t state);
void dhcp_set_callback(dhcp_callback_t callback);

/* === Main TCP/IP stack interface (from tcpip.h) === */


/* Hardware driver structure */
typedef struct {
    bool (*init)(void);
    bool (*send_packet)(const uint8_t *data, uint16_t len);
    uint16_t (*recv_packet)(uint8_t *buffer, uint16_t max_len);
    void (*get_mac_addr)(eth_addr_t *addr);
    void (*set_mac_addr)(eth_addr_t *addr);
    void (*set_ip_addr)(ip_addr_t *addr);
    void (*get_ip_addr)(ip_addr_t *addr);
    void (*set_subnet_mask)(ip_addr_t* mask);    
    void (*get_subnet_mask)(ip_addr_t* mask);    
    void (*set_gateway_addr)(ip_addr_t* addr);
    void (*get_gateway_addr)(ip_addr_t* addr);
    bool (*get_link_status)(void);
    void (*dump_status)(void);
} net_driver_t;

typedef struct {
    bool enable_arp;
    bool enable_dhcp;
    bool enable_ip;
    bool enable_udp;
    bool enable_icmp;
    bool enable_ethernet;
} tsiplib_config_t;

/* Stack initialization and management */
bool tcpip_init(net_driver_t *driver, tsiplib_config_t* config);
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

#endif /* TSIPLIB_H */