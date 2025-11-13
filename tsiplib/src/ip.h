#ifndef IP_H
#define IP_H

#include "types.h"
#include "net.h"

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

#endif /* IP_H */