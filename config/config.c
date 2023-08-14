#include "config.h"
#include "../utils/iniparser/iniparser.h"

struct config * config;

void config_init(){
    static struct config conf;
    conf.port = DEFAULT_PORT;
    conf.is_anonymously = DEFAULT_IS_ANONYMOUSLY;

    dictionary * ini;
    char * ini_name = CONFIG_FILE_DIR;

    ini = iniparser_load(ini_name);
    iniparser_dump(ini, stdout);

    conf.is_anonymously = iniparser_getboolean(ini, "login:anonymously", 0);

    if(conf.is_anonymously){
        conf.user_control = iniparser_getstring(ini, "login:user_control", NULL);
        conf.file_dir = iniparser_getstring(ini, "login:file_dir", NULL);
        conf.file_name = iniparser_getstring(ini, "login:file_name", NULL);
    }

    iniparser_freedict(ini);
    
    config = &conf;
}