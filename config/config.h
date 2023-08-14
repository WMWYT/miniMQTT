#ifndef CONFIG_H
#define CONFIG_H

#define CONFIG_FILE_DIR "./config.ini"
#define DEFAULT_PORT 1883
#define DEFAULT_IS_ANONYMOUSLY 0
#define BUFF_SIZE 128
#define EPOLL_SIZE 128

struct config{
    int port;
    int is_anonymously;
    const char * user_control;
    const char * file_dir;
    const char * file_name;
};

void config_init();

#endif