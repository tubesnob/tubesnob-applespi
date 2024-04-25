#include "spidl.h"
#include "../../tslib/src/getopt.h"

int parse_command_line(int argc, char** argv, config_t *config) {
    int c = 0;
    while (1)
    {
        int this_option_optind = optind ? optind : 1;
        c = getopt(argc, argv, "dc:b:vi:o:s");
        if (c == EOF)
            break;
        switch (c)
        {
            case 'd':
                // output file to stdout
                config->outputFileToSTDOUT = 1;
                break;
            case 'c':
                // config file
                config->config_file_name = strdup(optarg);
                break;
            case 'b':
                // block size
                config->packet_size = atoi(optarg);
                break;
            case 'v':
                // verbose
                _tslog->logMask |= TSLOG_LEVEL_VERBOSE;
                break;
            case 'i':
                // file name to read
                config->file_name = strdup(optarg);
                break;
            case 'o':
                // file name to save
                config->save_file_name = strdup(optarg);
                break;
            case 's':
                // silent mode
                _tslog->logMask = TSLOG_LEVEL_NONE;
                break;
        }
    }
    return 0;
}