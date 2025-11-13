#ifndef ICMP_H
#define ICMP_H

#include "types.h"
#include "net.h"

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

#endif /* ICMP_H */