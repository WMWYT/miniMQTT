#ifndef CONFIG_H
#define CONFIG_H

#define DEFAULT_INI_DIR "./config.ini"
#define DEFAULT_PORT 1883
#define BUFF_SIZE 128
#define EPOLL_SIZE 128

struct config{
    int epoll_size;
    int port;
};

struct config * config;

void config_init();

#endif