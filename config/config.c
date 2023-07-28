#include "config.h"
#include "../utils/iniparser/iniparser.h"

struct config conf;

void config_init(){
    conf.epoll_size = EPOLL_SIZE;
    conf.port = DEFAULT_PORT;

    dictionary * ini;
    char * ini_name = DEFAULT_INI_DIR;

    ini = iniparser_load(ini_name);
    iniparser_dump(ini, stdout);
    iniparser_freedict(ini);
    
    config = &conf;
}