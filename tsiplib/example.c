/* Example TCP/IP stack application for 65816 */

#include "include/tcpip.h"
#include <stdio.h>
#include <string.h>

/* Example hardware driver implementation */
/* This would be replaced with actual hardware driver code */

static eth_addr_t my_mac = {{0x00, 0x11, 0x22, 0x33, 0x44, 0x55}};
static uint8_t rx_buffer[ETH_MAX_FRAME_LEN];
static uint16_t rx_len = 0;

static bool example_init(void)
{
    printf("Network hardware initialized\n");
    return true;
}

static void example_get_mac(eth_addr_t *addr)
{
    eth_addr_copy(addr, &my_mac);
}

static bool example_send(const uint8_t *data, uint16_t len)
{
    printf("Sending %d bytes\n", len);
    /* In real driver, send data to hardware */
    return true;
}

static uint16_t example_recv(uint8_t *buffer, uint16_t max_len)
{
    /* In real driver, check hardware for received packets */
    if (rx_len > 0 && rx_len <= max_len) {
        memcpy(buffer, rx_buffer, rx_len);
        uint16_t len = rx_len;
        rx_len = 0;
        return len;
    }
    return 0;
}

static bool example_link_up(void)
{
    return true;  /* Always up in this example */
}

/* Network driver structure */
static net_driver_t example_driver = {
    example_init,
    example_get_mac,
    example_send,
    example_recv,
    example_link_up,
    NULL  /* No promiscuous mode */
};

/* DHCP state callback */
static void dhcp_state_changed(dhcp_state_t state)
{
    printf("DHCP state: ");
    switch (state) {
        case DHCP_STATE_INIT:
            printf("INIT\n");
            break;
        case DHCP_STATE_SELECTING:
            printf("SELECTING\n");
            break;
        case DHCP_STATE_REQUESTING:
            printf("REQUESTING\n");
            break;
        case DHCP_STATE_BOUND:
            printf("BOUND - Got IP address!\n");
            break;
        default:
            printf("Unknown\n");
    }
}

/* ICMP echo reply callback */
static void ping_reply(const ip_addr_t *src, uint16_t id, uint16_t seq, 
                      const uint8_t *data, uint16_t len)
{
    char ip_str[16];
    ip_addr_to_str(src, ip_str);
    printf("Ping reply from %s: seq=%d, len=%d\n", ip_str, seq, len);
}

/* UDP echo server callback */
static void udp_echo_rx(const uint8_t *data, uint16_t len, 
                       const ip_addr_t *src, uint16_t src_port)
{
    char ip_str[16];
    ip_addr_to_str(src, ip_str);
    printf("UDP echo from %s:%d - %d bytes\n", ip_str, src_port, len);
    /* Echo server would send the data back here */
}

int main(void)
{
    ip_addr_t my_ip, netmask, gateway;
    char ip_str[16], mac_str[18];
    eth_addr_t mac;
    udp_socket_t *echo_socket;
    dhcp_config_t dhcp_cfg;
    int use_dhcp = 1;  /* Set to 0 for static IP */
    
    printf("TCP/IP Stack Example for 65816\n");
    printf("==============================\n\n");
    
    /* Initialize TCP/IP stack */
    if (!tcpip_init(&example_driver)) {
        printf("Failed to initialize TCP/IP stack\n");
        return 1;
    }
    
    /* Get and display MAC address */
    tcpip_get_mac_address(&mac);
    mac_addr_to_str(&mac, mac_str);
    printf("MAC Address: %s\n", mac_str);
    
    if (use_dhcp) {
        /* Use DHCP to get IP configuration */
        printf("\nStarting DHCP...\n");
        dhcp_set_callback(dhcp_state_changed);
        
        if (!dhcp_start()) {
            printf("Failed to start DHCP\n");
            return 1;
        }
        
        /* Wait for DHCP to complete */
        while (dhcp_get_state() != DHCP_STATE_BOUND) {
            tcpip_poll();
            /* In real application, add delay here */
        }
        
        /* Get DHCP configuration */
        if (dhcp_get_config(&dhcp_cfg)) {
            ip_addr_to_str(&dhcp_cfg.ip, ip_str);
            printf("\nIP Address: %s\n", ip_str);
            
            ip_addr_to_str(&dhcp_cfg.netmask, ip_str);
            printf("Netmask: %s\n", ip_str);
            
            ip_addr_to_str(&dhcp_cfg.gateway, ip_str);
            printf("Gateway: %s\n", ip_str);
            
            ip_addr_to_str(&dhcp_cfg.dns, ip_str);
            printf("DNS Server: %s\n", ip_str);
            
            printf("Lease Time: %lu seconds\n", dhcp_cfg.lease_time);
        }
    } else {
        /* Use static IP configuration */
        ip_addr_from_str("192.168.1.100", &my_ip);
        ip_addr_from_str("255.255.255.0", &netmask);
        ip_addr_from_str("192.168.1.1", &gateway);
        
        if (!tcpip_set_ip_config(&my_ip, &netmask, &gateway)) {
            printf("Failed to set IP configuration\n");
            return 1;
        }
        
        printf("\nStatic IP configuration:\n");
        printf("IP Address: 192.168.1.100\n");
        printf("Netmask: 255.255.255.0\n");
        printf("Gateway: 192.168.1.1\n");
    }
    
    /* Set up ICMP echo reply handler */
    icmp_set_echo_callback(ping_reply);
    
    /* Create UDP echo server on port 7 */
    echo_socket = udp_socket_create();
    if (echo_socket) {
        if (udp_socket_bind(echo_socket, 7)) {
            udp_socket_set_rx_callback(echo_socket, udp_echo_rx);
            printf("\nUDP echo server listening on port 7\n");
        }
    }
    
    /* Send a ping to gateway */
    printf("\nSending ping to gateway...\n");
    tcpip_get_ip_config(&my_ip, &netmask, &gateway);
    icmp_send_echo_request(&gateway, 1, 1, (uint8_t *)"Hello", 5);
    
    printf("\nMain loop running. Press Ctrl+C to exit.\n");
    printf("The stack will:\n");
    printf("- Respond to ARP requests\n");
    printf("- Reply to ping (ICMP echo) requests\n");
    printf("- Echo UDP packets sent to port 7\n\n");
    
    /* Main loop */
    while (1) {
        tcpip_poll();
        /* In real application, add other tasks here */
    }
    
    return 0;
}