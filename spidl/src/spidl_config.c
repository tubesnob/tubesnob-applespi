#include "spidl.h"

static int config_handler(void* user, const char* section, const char* name, const char* value);

int config_load(const char* configFileName, config_t* config) {
    return ini_parse(configFileName, config_handler, config);
}

#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
static int config_handler(void* user, const char* section, const char* name, const char* value)
{
    config_t* pconfig = (config_t*) user;

    if (MATCH("source", "ip"))
        pconfig->source_ipaddr = strdup(value);

    if (MATCH("source", "mac"))
        pconfig->source_macaddr = strdup(value);

    if (MATCH("source", "gw"))
        pconfig->source_gwaddr = strdup(value);

    if (MATCH("source", "mask"))
        pconfig->source_mask = strdup(value);

    if (MATCH("source", "port"))
        pconfig->source_port = atoi(value);

    if (MATCH("dest", "ip"))
        pconfig->dest_ip = strdup(value);

    if (MATCH("dest", "port"))
        pconfig->dest_port = atoi(value);

    if (MATCH("transfer", "packetSize"))
        pconfig->packet_size = atoi(value);

    return 1;
}

config_t* config_cleanup(config_t* config) 
{
        if (config) {
                freeandnull(config->dest_ip);
                freeandnull(config->file_name);
                freeandnull(config->save_file_name);
                freeandnull(config->source_gwaddr);
                freeandnull(config->source_ipaddr);
                freeandnull(config->source_macaddr);
                freeandnull(config->source_mask);
                freeandnull(config);
        }
        return NULL;
}
