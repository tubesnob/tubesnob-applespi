#ifndef HAL_H
#define HAL_H

#include "types.h"
#include "net.h"

/* Hardware abstraction layer interface */
/* This allows the TCP/IP stack to work with different network hardware */

/* Hardware driver structure */
typedef struct {
    /* Initialize the network hardware */
    bool (*init)(void);
    
    /* Get the MAC address */
    void (*get_mac_addr)(eth_addr_t *addr);
    
    /* Send a packet */
    bool (*send_packet)(const uint8_t *data, uint16_t len);
    
    /* Receive a packet (non-blocking) */
    /* Returns number of bytes received, 0 if no packet available */
    uint16_t (*recv_packet)(uint8_t *buffer, uint16_t max_len);
    
    /* Check if link is up */
    bool (*link_up)(void);
    
    /* Optional: Set promiscuous mode */
    void (*set_promiscuous)(bool enable);
} net_driver_t;

/* Global driver instance - must be provided by the application */
extern net_driver_t *net_driver;

/* Helper functions */
bool hal_init(void);
void hal_get_mac_addr(eth_addr_t *addr);
bool hal_send_packet(const uint8_t *data, uint16_t len);
uint16_t hal_recv_packet(uint8_t *buffer, uint16_t max_len);
bool hal_link_up(void);

#endif /* HAL_H */