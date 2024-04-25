#include "spiws.h"
#include "../../orcadefaults.h"

//#pragma noroot

#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0

int config_handler(void* user, const char* section, const char* name, const char* value)
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

    if (MATCH("source", "listenport"))
        pconfig->source_listenport = atoi(value);

    if (MATCH("serve", "rootfolder"))
        pconfig->root_folder = strdup(value);

    return 1;
}
