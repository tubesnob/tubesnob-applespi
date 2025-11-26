#include "tsiplib.h"
#include <string.h>

extern net_driver_t* _driver;

/* Maximum number of UDP sockets */
#define MAX_UDP_SOCKETS 8

/* UDP socket pool */
static udp_socket_t sockets[MAX_UDP_SOCKETS];

/* Forward declarations */
static void udp_rx_callback(const uint8_t *data, uint16_t len, const ip_addr_t *src);
static uint16_t udp_checksum(const ip_addr_t *src, const ip_addr_t *dest, const uint8_t *data, uint16_t len);

bool udp_init(void)
{
    /* Clear socket pool */
    memset(sockets, 0, sizeof(sockets));
    
    /* Register for UDP packets */
    ip_register_protocol(IP_PROTO_UDP, udp_rx_callback);
    
    return true;
}

udp_socket_t* udp_socket_create(void)
{
    uint8_t i;
    
    /* Find free socket */
    for (i = 0; i < MAX_UDP_SOCKETS; i++) {
        if (!sockets[i].in_use) {
            memset(&sockets[i], 0, sizeof(udp_socket_t));
            sockets[i].in_use = true;
            return &sockets[i];
        }
    }
    
    return NULL;
}

bool udp_socket_bind(udp_socket_t *socket, uint16_t port)
{
    uint8_t i;
    
    if (!socket || !socket->in_use || port == 0) {
        return false;
    }
    
    /* Check if port is already in use */
    for (i = 0; i < MAX_UDP_SOCKETS; i++) {
        if (&sockets[i] != socket && sockets[i].in_use && sockets[i].local_port == port) {
            return false;
        }
    }
    
    socket->local_port = port;
    return true;
}

bool udp_socket_connect(udp_socket_t *socket, const ip_addr_t *remote_ip, uint16_t remote_port)
{
    if (!socket || !socket->in_use || !remote_ip || remote_port == 0) {
        return false;
    }
    
    ip_addr_copy(&socket->remote_ip, remote_ip);
    socket->remote_port = remote_port;
    
    return true;
}

bool udp_send(udp_socket_t *socket, const uint8_t *data, uint16_t len)
{
    if (!socket || !socket->in_use || socket->remote_port == 0) {
        return false;
    }
    
    return udp_sendto(socket, data, len, &socket->remote_ip, socket->remote_port);
}

bool udp_sendto(udp_socket_t *socket, const uint8_t *data, uint16_t len, const ip_addr_t *dest_ip, uint16_t dest_port)
{
    uint8_t packet[ETH_MTU - sizeof(ip_header_t)];
    udp_header_t *udp;
    uint16_t total_len;
    ip_addr_t src_ip;
    
    if (!socket || !socket->in_use || !data || !dest_ip || dest_port == 0 || 
        len > (sizeof(packet) - sizeof(udp_header_t))) {
        return false;
    }
    
    /* Build UDP header */
    udp = (udp_header_t *)packet;
    udp->src_port = htons(socket->local_port);
    udp->dest_port = htons(dest_port);
    total_len = sizeof(udp_header_t) + len;
    udp->length = htons(total_len);
    udp->checksum = 0;  /* Optional for IPv4 */
    
    /* Copy payload */
    memcpy(packet + sizeof(udp_header_t), data, len);
    
    /* Calculate checksum (optional but recommended) */
    ip_get_address(&src_ip);
    udp->checksum = udp_checksum(&src_ip, dest_ip, packet, total_len);
    
    /* Send packet */
    return ip_send(dest_ip, IP_PROTO_UDP, packet, total_len);
}

void udp_socket_close(udp_socket_t *socket)
{
    if (socket && socket->in_use) {
        memset(socket, 0, sizeof(udp_socket_t));
    }
}

void udp_socket_set_rx_callback(udp_socket_t *socket, 
    void (*callback)(const uint8_t *data, uint16_t len, const ip_addr_t *src, uint16_t src_port))
{
    if (socket && socket->in_use) {
        socket->rx_callback = callback;
    }
}

static void udp_rx_callback(const uint8_t *data, uint16_t len, const ip_addr_t *src)
{
    udp_header_t *udp;
    uint16_t udp_len;
    uint16_t src_port, dest_port;
    uint8_t i;
    
    if (len < sizeof(udp_header_t)) {
        return;
    }
    
    udp = (udp_header_t *)data;
    
    /* Check UDP length */
    udp_len = ntohs(udp->length);
    if (udp_len < sizeof(udp_header_t) || udp_len > len) {
        return;
    }
    
    /* Get ports */
    src_port = ntohs(udp->src_port);
    dest_port = ntohs(udp->dest_port);
    
    /* Find matching socket */
    for (i = 0; i < MAX_UDP_SOCKETS; i++) {
        if (sockets[i].in_use && sockets[i].local_port == dest_port) {
            /* Check if socket is connected and source matches */
            if (sockets[i].remote_port != 0) {
                if (sockets[i].remote_port != src_port ||
                    !ip_addr_is_equal(&sockets[i].remote_ip, src)) {
                    continue;
                }
            }
            
            /* Call receive callback */
            if (sockets[i].rx_callback) {
                sockets[i].rx_callback(data + sizeof(udp_header_t),
                                     udp_len - sizeof(udp_header_t),
                                     src,
                                     src_port);
            }
            break;
        }
    }
}

static uint16_t udp_checksum(const ip_addr_t *src, const ip_addr_t *dest, const uint8_t *data, uint16_t len)
{
    uint32_t sum = 0;
    uint16_t *ptr;
    uint8_t pseudo_header[12];
    
    /* Build pseudo header */
    memcpy(pseudo_header, src->addr, 4);
    memcpy(pseudo_header + 4, dest->addr, 4);
    pseudo_header[8] = 0;
    pseudo_header[9] = IP_PROTO_UDP;
    *(uint16_t *)(pseudo_header + 10) = htons(len);
    
    /* Sum pseudo header */
    ptr = (uint16_t *)pseudo_header;
    sum += ptr[0] + ptr[1] + ptr[2] + ptr[3] + ptr[4] + ptr[5];
    
    /* Sum UDP header and data */
    ptr = (uint16_t *)data;
    while (len > 1) {
        sum += *ptr++;
        len -= 2;
    }
    
    /* Add left-over byte, if any */
    if (len > 0) {
        sum += *(uint8_t *)ptr;
    }
    
    /* Add carries */
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }
    
    /* One's complement */
    return ~sum;
}