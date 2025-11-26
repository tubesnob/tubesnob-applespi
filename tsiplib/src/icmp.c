#include "tsiplib.h"
#include <string.h>

extern net_driver_t* _driver;

/* Echo reply callback */
static icmp_echo_callback_t echo_callback = NULL;

/* Forward declarations */
static void icmp_rx_callback(const uint8_t *data, uint16_t len, const ip_addr_t *src);
static uint16_t icmp_checksum(const uint8_t *data, uint16_t len);

bool icmp_init(void)
{
    /* Register for ICMP packets */
    ip_register_protocol(IP_PROTO_ICMP, icmp_rx_callback);
    
    return true;
}

bool icmp_send_echo_request(const ip_addr_t *dest, uint16_t id, uint16_t seq, const uint8_t *data, uint16_t len)
{
    uint8_t packet[ETH_MTU - sizeof(ip_header_t)];
    icmp_header_t *icmp;
    uint16_t total_len;
    
    if (!dest || len > (sizeof(packet) - sizeof(icmp_header_t))) {
        return false;
    }
    
    /* Build ICMP header */
    icmp = (icmp_header_t *)packet;
    icmp->type = ICMP_TYPE_ECHO_REQUEST;
    icmp->code = 0;
    icmp->checksum = 0;
    icmp->identifier = htons(id);
    icmp->sequence = htons(seq);
    
    /* Copy payload if any */
    if (data && len > 0) {
        memcpy(packet + sizeof(icmp_header_t), data, len);
    }
    
    /* Calculate total length */
    total_len = sizeof(icmp_header_t) + len;
    
    /* Calculate checksum */
    icmp->checksum = icmp_checksum(packet, total_len);
    
    /* Send packet */
    return ip_send(dest, IP_PROTO_ICMP, packet, total_len);
}

bool icmp_send_echo_reply(const ip_addr_t *dest, uint16_t id, uint16_t seq, const uint8_t *data, uint16_t len)
{
    uint8_t packet[ETH_MTU - sizeof(ip_header_t)];
    icmp_header_t *icmp;
    uint16_t total_len;
    
    if (!dest || len > (sizeof(packet) - sizeof(icmp_header_t))) {
        return false;
    }
    
    /* Build ICMP header */
    icmp = (icmp_header_t *)packet;
    icmp->type = ICMP_TYPE_ECHO_REPLY;
    icmp->code = 0;
    icmp->checksum = 0;
    icmp->identifier = htons(id);
    icmp->sequence = htons(seq);
    
    /* Copy payload if any */
    if (data && len > 0) {
        memcpy(packet + sizeof(icmp_header_t), data, len);
    }
    
    /* Calculate total length */
    total_len = sizeof(icmp_header_t) + len;
    
    /* Calculate checksum */
    icmp->checksum = icmp_checksum(packet, total_len);
    
    /* Send packet */
    return ip_send(dest, IP_PROTO_ICMP, packet, total_len);
}

void icmp_set_echo_callback(icmp_echo_callback_t callback)
{
    echo_callback = callback;
}

static void icmp_rx_callback(const uint8_t *data, uint16_t len, const ip_addr_t *src)
{
    icmp_header_t *icmp;
    
    if (len < sizeof(icmp_header_t)) {
        return;
    }
    
    icmp = (icmp_header_t *)data;
    
    /* Verify checksum */
    if (icmp_checksum(data, len) != 0) {
        return;
    }
    
    /* Handle different ICMP types */
    switch (icmp->type) {
        case ICMP_TYPE_ECHO_REQUEST:
            /* Send echo reply */
            icmp_send_echo_reply(src, 
                               ntohs(icmp->identifier), 
                               ntohs(icmp->sequence),
                               data + sizeof(icmp_header_t), 
                               len - sizeof(icmp_header_t));
            break;
            
        case ICMP_TYPE_ECHO_REPLY:
            /* Call callback if registered */
            if (echo_callback) {
                echo_callback(src,
                            ntohs(icmp->identifier),
                            ntohs(icmp->sequence),
                            data + sizeof(icmp_header_t),
                            len - sizeof(icmp_header_t));
            }
            break;
            
        default:
            /* Ignore other types for now */
            break;
    }
}

static uint16_t icmp_checksum(const uint8_t *data, uint16_t len)
{
    /* ICMP uses the same checksum algorithm as IP */
    return ip_checksum(data, len);
}