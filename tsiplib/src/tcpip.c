#include "tsiplib.h"
#include <string.h>

/* Stack state */
static bool _initialized = false;
static tsiplib_config_t* _config;
net_driver_t* _driver;

bool tcpip_init(net_driver_t *driver, tsiplib_config_t* config)
{
    _config = config;
    _driver = driver;

    if (_initialized || !driver) {
        return false;
    }
    
    if (_config->enable_ethernet) {
        if (!eth_init()) {
            return false;
        }
    }
    
    if (_config->enable_arp && _config->enable_ethernet) { 
        if (!arp_init()) {
                return false;
        }
    } 
    
    if (_config->enable_ip && _config->enable_ethernet) {
        if (!ip_init()) {
            return false;
        }
    }
    
    if (_config->enable_icmp && _config->enable_ip && _config->enable_ethernet) {
        if (!icmp_init()) {
            return false;
        }
    }
    
    if (_config->enable_udp && _config->enable_ip && _config->enable_ethernet) {
        if (!udp_init()) {
            return false;
        }
    }
    
    if (_config->enable_dhcp && _config->enable_ip && _config->enable_ethernet) {
        if (!dhcp_init()) {
            return false;
        }
    }
    
    _initialized = true;

    return true;
}

void tcpip_poll(void)
{
    if (!_initialized) {
        return;
    }
    
    /* Poll Ethernet layer for incoming packets */
    if (_config->enable_ethernet) {
        eth_poll();
    }
    
    /* Poll DHCP if active */
    if (_config->enable_dhcp) {
        dhcp_poll();
    }
}

void tcpip_shutdown(void)
{
    if (!_initialized) {
        return;
    }
    if (_config->enable_dhcp) {
        dhcp_stop();
    }
    if (_config->enable_arp) {
        arp_cache_flush();
    }
    _initialized = false;
}

bool tcpip_set_ip_config(const ip_addr_t *ip, const ip_addr_t *netmask, const ip_addr_t *gateway)
{
    if (!_initialized || !ip || !netmask || !gateway) {
        return false;
    }
    
    ip_set_address(ip);
    ip_set_netmask(netmask);
    ip_set_gateway(gateway);
    
    return true;
}

bool tcpip_get_ip_config(ip_addr_t *ip, ip_addr_t *netmask, ip_addr_t *gateway)
{
    if (!_initialized || !ip || !netmask || !gateway) {
        return false;
    }
    
    ip_get_address(ip);
    ip_get_netmask(netmask);
    ip_get_gateway(gateway);
    
    return true;
}

void tcpip_get_mac_address(eth_addr_t *mac)
{
    if (mac) {
        _driver->get_mac_addr(mac);
    }
}

bool tcpip_is_link_up(void)
{
    return _driver->get_link_status();
}

bool ip_addr_from_str(const char *str, ip_addr_t *addr)
{
    uint8_t i, j;
    uint16_t val;
    
    if (!str || !addr) {
        return false;
    }
    
    j = 0;
    val = 0;
    
    for (i = 0; i < 4; i++) {
        /* Parse octet */
        while (*str >= '0' && *str <= '9') {
            val = val * 10 + (*str - '0');
            if (val > 255) {
                return false;
            }
            str++;
        }
        
        addr->addr[i] = (uint8_t)val;
        val = 0;
        
        /* Check separator */
        if (i < 3) {
            if (*str != '.') {
                return false;
            }
            str++;
        }
    }
    
    /* Check for end of string */
    return (*str == '\0');
}

void ip_addr_to_str(const ip_addr_t *addr, char *str)
{
    uint8_t i, j;
    uint8_t val;
    char tmp[4];
    
    if (!addr || !str) {
        return;
    }
    
    *str = '\0';
    
    for (i = 0; i < 4; i++) {
        val = addr->addr[i];
        j = 0;
        
        /* Convert to decimal */
        if (val >= 100) {
            tmp[j++] = '0' + (val / 100);
            val %= 100;
        }
        if (val >= 10 || j > 0) {
            tmp[j++] = '0' + (val / 10);
            val %= 10;
        }
        tmp[j++] = '0' + val;
        tmp[j] = '\0';
        
        /* Copy to output */
        strcat(str, tmp);
        
        if (i < 3) {
            strcat(str, ".");
        }
    }
}

void mac_addr_to_str(const eth_addr_t *addr, char *str)
{
    uint8_t i;
    static const char hex[] = "0123456789ABCDEF";
    
    if (!addr || !str) {
        return;
    }
    
    for (i = 0; i < ETH_ADDR_LEN; i++) {
        *str++ = hex[addr->addr[i] >> 4];
        *str++ = hex[addr->addr[i] & 0x0F];
        if (i < ETH_ADDR_LEN - 1) {
            *str++ = ':';
        }
    }
    *str = '\0';
}