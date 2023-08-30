#include "config.h"
#include <signal.h>
#include "../utils/iniparser/iniparser.h"

struct config * config;

void config_init(){
    static struct config conf;
    dictionary * ini;
    char * ini_name = CONFIG_FILE_DIR;

    ini = iniparser_load(ini_name);
    iniparser_dump(ini, stdout);

    conf.port = iniparser_getint(ini, "info:port", DEFAULT_PORT);

    strcpy(conf.system_dir, iniparser_getstring(ini, "system:dir", NULL));
    if(conf.system_dir == NULL){
        printf("config error you not have set [system:dir]");
        exit(0);
    }

    conf.is_anonymously = iniparser_getboolean(ini, "login:anonymously", DEFAULT_IS_ANONYMOUSLY);

    if(conf.is_anonymously){
        strcpy(conf.control_type, iniparser_getstring(ini, "login:control_type", NULL));
        if(conf.control_type == NULL){
            printf("config error you not have set [control_type].\n");
            exit(0);
        }

        strcpy(conf.dir, iniparser_getstring(ini, "control:dir", NULL));
        if(conf.dir == NULL){
            printf("config error you not have set [dir].\n");
            exit(0);
        }
    }

    iniparser_freedict(ini);
    
    config = &conf;
}