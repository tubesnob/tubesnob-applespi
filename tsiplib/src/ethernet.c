#include "ethernet.h"
#include "hal.h"
#include <string.h>

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
    /* Get our MAC address from hardware */
    hal_get_mac_addr(&our_mac);
    
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
    
    /* Send frame */
    return hal_send_packet(frame, frame_len);
}

void eth_poll(void)
{
    uint16_t len;
    eth_header_t *hdr;
    uint16_t type;
    uint8_t i;
    
    /* Receive packet */
    len = hal_recv_packet(rx_buffer, sizeof(rx_buffer));
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