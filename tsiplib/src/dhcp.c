#include "tsiplib.h"
#include <string.h>

extern net_driver_t* _driver;

/* DHCP message types */
#define DHCP_MSG_DISCOVER   1
#define DHCP_MSG_OFFER      2
#define DHCP_MSG_REQUEST    3
#define DHCP_MSG_DECLINE    4
#define DHCP_MSG_ACK        5
#define DHCP_MSG_NAK        6
#define DHCP_MSG_RELEASE    7

/* DHCP options */
#define DHCP_OPT_PAD        0
#define DHCP_OPT_SUBNET     1
#define DHCP_OPT_ROUTER     3
#define DHCP_OPT_DNS        6
#define DHCP_OPT_HOSTNAME   12
#define DHCP_OPT_REQ_IP     50
#define DHCP_OPT_LEASE_TIME 51
#define DHCP_OPT_MSG_TYPE   53
#define DHCP_OPT_SERVER_ID  54
#define DHCP_OPT_PARAM_LIST 55
#define DHCP_OPT_END        255

/* DHCP header */
typedef struct {
    uint8_t  op;
    uint8_t  htype;
    uint8_t  hlen;
    uint8_t  hops;
    uint32_t xid;
    uint16_t secs;
    uint16_t flags;
    ip_addr_t ciaddr;
    ip_addr_t yiaddr;
    ip_addr_t siaddr;
    ip_addr_t giaddr;
    uint8_t  chaddr[16];
    uint8_t  sname[64];
    uint8_t  file[128];
    uint8_t  options[312];  /* Includes magic cookie */
} dhcp_msg_t;

/* DHCP magic cookie */
static const uint8_t dhcp_magic[] = {99, 130, 83, 99};

/* DHCP state */
static dhcp_state_t state = DHCP_STATE_INIT;
static dhcp_config_t config;
static uint32_t transaction_id = 0;
static ip_addr_t server_ip;
static udp_socket_t *dhcp_socket = NULL;
static dhcp_callback_t state_callback = NULL;
static uint16_t retry_count = 0;

/* Forward declarations */
static void dhcp_rx_callback(const uint8_t *data, uint16_t len, const ip_addr_t *src, uint16_t src_port);
static bool dhcp_send_discover(void);
static bool dhcp_send_request(void);
static uint8_t* dhcp_add_option(uint8_t *ptr, uint8_t option, uint8_t len, const void *data);
static uint8_t* dhcp_find_option(uint8_t *options, uint16_t len, uint8_t option);

bool dhcp_init(void)
{
    /* Clear state */
    state = DHCP_STATE_INIT;
    memset(&config, 0, sizeof(config));
    transaction_id = 0;
    retry_count = 0;
    
    return true;
}

bool dhcp_start(void)
{
    eth_addr_t mac;
    
    /* Create UDP socket */
    dhcp_socket = udp_socket_create();
    if (!dhcp_socket) {
        return false;
    }
    
    /* Bind to DHCP client port */
    if (!udp_socket_bind(dhcp_socket, DHCP_CLIENT_PORT)) {
        udp_socket_close(dhcp_socket);
        dhcp_socket = NULL;
        return false;
    }
    
    /* Set receive callback */
    udp_socket_set_rx_callback(dhcp_socket, dhcp_rx_callback);
    
    
    /* Generate transaction ID from MAC address */
    _driver->get_mac_addr(&mac);
    transaction_id = (mac.addr[2] << 24) | (mac.addr[3] << 16) | 
                     (mac.addr[4] << 8) | mac.addr[5];
    
    /* Send DHCP discover */
    state = DHCP_STATE_SELECTING;
    retry_count = 0;
    
    if (state_callback) {
        state_callback(state);
    }
    
    return dhcp_send_discover();
}

void dhcp_stop(void)
{
    if (dhcp_socket) {
        udp_socket_close(dhcp_socket);
        dhcp_socket = NULL;
    }
    
    state = DHCP_STATE_INIT;
    memset(&config, 0, sizeof(config));
}

bool dhcp_poll(void)
{
    /* Handle retransmissions */
    if (state == DHCP_STATE_SELECTING || state == DHCP_STATE_REQUESTING) {
        retry_count++;
        
        if (retry_count > 300) {  /* About 30 seconds at 100ms poll rate */
            /* Timeout, restart */
            state = DHCP_STATE_SELECTING;
            retry_count = 0;
            dhcp_send_discover();
        } else if ((retry_count % 30) == 0) {  /* Retry every 3 seconds */
            if (state == DHCP_STATE_SELECTING) {
                dhcp_send_discover();
            } else {
                dhcp_send_request();
            }
        }
    }
    
    return (state == DHCP_STATE_BOUND);
}

dhcp_state_t dhcp_get_state(void)
{
    return state;
}

bool dhcp_get_config(dhcp_config_t *cfg)
{
    if (!cfg || state != DHCP_STATE_BOUND) {
        return false;
    }
    
    memcpy(cfg, &config, sizeof(dhcp_config_t));
    return true;
}

void dhcp_set_callback(dhcp_callback_t callback)
{
    state_callback = callback;
}

static void dhcp_rx_callback(const uint8_t *data, uint16_t len, const ip_addr_t *src, uint16_t src_port)
{
    dhcp_msg_t *msg;
    uint8_t *opt_ptr;
    uint8_t msg_type = 0;
    
    if (len < sizeof(dhcp_msg_t) || src_port != DHCP_SERVER_PORT) {
        return;
    }
    
    msg = (dhcp_msg_t *)data;
    
    /* Check transaction ID */
    if (ntohl(msg->xid) != transaction_id) {
        return;
    }
    
    /* Check magic cookie */
    if (memcmp(msg->options, dhcp_magic, 4) != 0) {
        return;
    }
    
    /* Find message type option */
    opt_ptr = dhcp_find_option(msg->options + 4, sizeof(msg->options) - 4, DHCP_OPT_MSG_TYPE);
    if (!opt_ptr || opt_ptr[1] != 1) {
        return;
    }
    msg_type = opt_ptr[2];
    
    /* Handle message based on current state */
    switch (state) {
        case DHCP_STATE_SELECTING:
            if (msg_type == DHCP_MSG_OFFER) {
                /* Save offered configuration */
                ip_addr_copy(&config.ip, &msg->yiaddr);
                ip_addr_copy(&server_ip, src);
                
                /* Extract other options */
                opt_ptr = dhcp_find_option(msg->options + 4, sizeof(msg->options) - 4, DHCP_OPT_SUBNET);
                if (opt_ptr && opt_ptr[1] == 4) {
                    memcpy(&config.netmask, opt_ptr + 2, 4);
                }
                
                opt_ptr = dhcp_find_option(msg->options + 4, sizeof(msg->options) - 4, DHCP_OPT_ROUTER);
                if (opt_ptr && opt_ptr[1] == 4) {
                    memcpy(&config.gateway, opt_ptr + 2, 4);
                }
                
                opt_ptr = dhcp_find_option(msg->options + 4, sizeof(msg->options) - 4, DHCP_OPT_DNS);
                if (opt_ptr && opt_ptr[1] >= 4) {
                    memcpy(&config.dns, opt_ptr + 2, 4);
                }
                
                opt_ptr = dhcp_find_option(msg->options + 4, sizeof(msg->options) - 4, DHCP_OPT_LEASE_TIME);
                if (opt_ptr && opt_ptr[1] == 4) {
                    config.lease_time = ntohl(*(uint32_t *)(opt_ptr + 2));
                }
                
                /* Send request */
                state = DHCP_STATE_REQUESTING;
                retry_count = 0;
                dhcp_send_request();
                
                if (state_callback) {
                    state_callback(state);
                }
            }
            break;
            
        case DHCP_STATE_REQUESTING:
            if (msg_type == DHCP_MSG_ACK) {
                /* Configuration accepted */
                state = DHCP_STATE_BOUND;
                
                /* Apply configuration */
                ip_set_address(&config.ip);
                ip_set_netmask(&config.netmask);
                ip_set_gateway(&config.gateway);
                
                if (state_callback) {
                    state_callback(state);
                }
            } else if (msg_type == DHCP_MSG_NAK) {
                /* Configuration rejected, restart */
                state = DHCP_STATE_SELECTING;
                retry_count = 0;
                dhcp_send_discover();
                
                if (state_callback) {
                    state_callback(state);
                }
            }
            break;
            
        default:
            break;
    }
}

static bool dhcp_send_discover(void)
{
    dhcp_msg_t msg;
    eth_addr_t mac;
    uint8_t *opt_ptr;
    ip_addr_t broadcast = {{255, 255, 255, 255}};
    
    /* Build DHCP discover message */
    memset(&msg, 0, sizeof(msg));
    msg.op = 1;  /* BOOTREQUEST */
    msg.htype = 1;  /* Ethernet */
    msg.hlen = 6;
    msg.xid = htonl(transaction_id);
    msg.flags = htons(0x8000);  /* Broadcast flag */
    
    /* Set hardware address */
    _driver->get_mac_addr(&mac);
    memcpy(msg.chaddr, mac.addr, ETH_ADDR_LEN);
    
    /* Add magic cookie */
    memcpy(msg.options, dhcp_magic, 4);
    opt_ptr = msg.options + 4;
    
    /* Add message type option */
    opt_ptr = dhcp_add_option(opt_ptr, DHCP_OPT_MSG_TYPE, 1, &(uint8_t){DHCP_MSG_DISCOVER});
    
    /* Add parameter request list */
    uint8_t params[] = {DHCP_OPT_SUBNET, DHCP_OPT_ROUTER, DHCP_OPT_DNS, DHCP_OPT_LEASE_TIME};
    opt_ptr = dhcp_add_option(opt_ptr, DHCP_OPT_PARAM_LIST, sizeof(params), params);
    
    /* Add end option */
    *opt_ptr++ = DHCP_OPT_END;
    
    /* Send packet */
    return udp_sendto(dhcp_socket, (uint8_t *)&msg, sizeof(msg), &broadcast, DHCP_SERVER_PORT);
}

static bool dhcp_send_request(void)
{
    dhcp_msg_t msg;
    eth_addr_t mac;
    uint8_t *opt_ptr;
    ip_addr_t broadcast = {{255, 255, 255, 255}};
    
    /* Build DHCP request message */
    memset(&msg, 0, sizeof(msg));
    msg.op = 1;  /* BOOTREQUEST */
    msg.htype = 1;  /* Ethernet */
    msg.hlen = 6;
    msg.xid = htonl(transaction_id);
    msg.flags = htons(0x8000);  /* Broadcast flag */
    
    /* Set hardware address */
    _driver->get_mac_addr(&mac);
    memcpy(msg.chaddr, mac.addr, ETH_ADDR_LEN);
    
    /* Add magic cookie */
    memcpy(msg.options, dhcp_magic, 4);
    opt_ptr = msg.options + 4;
    
    /* Add message type option */
    opt_ptr = dhcp_add_option(opt_ptr, DHCP_OPT_MSG_TYPE, 1, &(uint8_t){DHCP_MSG_REQUEST});
    
    /* Add requested IP */
    opt_ptr = dhcp_add_option(opt_ptr, DHCP_OPT_REQ_IP, 4, config.ip.addr);
    
    /* Add server identifier */
    opt_ptr = dhcp_add_option(opt_ptr, DHCP_OPT_SERVER_ID, 4, server_ip.addr);
    
    /* Add end option */
    *opt_ptr++ = DHCP_OPT_END;
    
    /* Send packet */
    return udp_sendto(dhcp_socket, (uint8_t *)&msg, sizeof(msg), &broadcast, DHCP_SERVER_PORT);
}

static uint8_t* dhcp_add_option(uint8_t *ptr, uint8_t option, uint8_t len, const void *data)
{
    *ptr++ = option;
    *ptr++ = len;
    if (len > 0 && data) {
        memcpy(ptr, data, len);
        ptr += len;
    }
    return ptr;
}

static uint8_t* dhcp_find_option(uint8_t *options, uint16_t len, uint8_t option)
{
    uint16_t i = 0;
    
    while (i < len) {
        if (options[i] == DHCP_OPT_END) {
            break;
        }
        
        if (options[i] == DHCP_OPT_PAD) {
            i++;
            continue;
        }
        
        if (options[i] == option) {
            return &options[i];
        }
        
        i += 2 + options[i + 1];
    }
    
    return NULL;
}