#ifndef ETHERNET_H
#define ETHERNET_H

#include "types.h"
#include "net.h"

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

#endif /* ETHERNET_H */