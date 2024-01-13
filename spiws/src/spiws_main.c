#include "spiws.h";
#include "../../orcadefaults.h"

char* __spiws_config_file_name="spiws.cfg";


int main(int argc, char** argv)
{
        config_t config;

        if (ini_parse(__spiws_config_file_name, config_handler, &config) < 0) {
                _tslog->info("Couldn't find config file [%s]\n",__spiws_config_file_name);
                goto exiterror;
        }

        if (spiws_init_w5500(&config)) {
                _tslog->info("Error during w5500 initialization\n");
                goto exiterror;
        }

        spiws_server_run(&config);

        return 0;

exiterror:

        _tslog->info("Error. Exiting.");
        return 1;
}



