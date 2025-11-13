#include "hal.h"

/* Global driver instance */
net_driver_t *net_driver = NULL;

bool hal_init(void)
{
    if (!net_driver || !net_driver->init) {
        return false;
    }
    return net_driver->init();
}

void hal_get_mac_addr(eth_addr_t *addr)
{
    if (net_driver && net_driver->get_mac_addr && addr) {
        net_driver->get_mac_addr(addr);
    }
}

bool hal_send_packet(const uint8_t *data, uint16_t len)
{
    if (!net_driver || !net_driver->send_packet || !data || len == 0) {
        return false;
    }
    return net_driver->send_packet(data, len);
}

uint16_t hal_recv_packet(uint8_t *buffer, uint16_t max_len)
{
    if (!net_driver || !net_driver->recv_packet || !buffer || max_len == 0) {
        return 0;
    }
    return net_driver->recv_packet(buffer, max_len);
}

bool hal_link_up(void)
{
    if (!net_driver || !net_driver->link_up) {
        return false;
    }
    return net_driver->link_up();
}