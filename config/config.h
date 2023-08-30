#ifndef CONFIG_H
#define CONFIG_H

#define CONFIG_FILE_DIR "./config.ini"
#define DEFAULT_PORT 1883
#define DEFAULT_IS_ANONYMOUSLY 0
#define BUFF_SIZE 128
#define EPOLL_SIZE 128

struct config{
    int port;
    char system_dir[256];
    int is_anonymously;
    char control_type[64];
    char dir[256];
};

void config_init();

#endif