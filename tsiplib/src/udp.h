#ifndef UDP_H
#define UDP_H

#include "types.h"
#include "net.h"

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

#endif /* UDP_H */