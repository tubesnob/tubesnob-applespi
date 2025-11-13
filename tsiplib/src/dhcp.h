#ifndef DHCP_H
#define DHCP_H

#include "types.h"
#include "net.h"

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

#endif /* DHCP_H */