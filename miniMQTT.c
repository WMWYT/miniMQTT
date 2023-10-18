#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <signal.h>
#include "net/net.h"
#include "net/session.h"
#include "config/config.h"

void printf_help(){
    printf("miniMQTT [-p port]\n");
    printf("-p : 程序所监听端口默认为1883.\n");
}

void printf_logo(){
    printf("███    ███ ██ ███    ██ ██ ███    ███  ██████  ████████ ████████\n");
    printf("████  ████    ████   ██    ████  ████ ██    ██    ██       ██   \n");
    printf("██ ████ ██ ██ ██ ██  ██ ██ ██ ████ ██ ██    ██    ██       ██   \n");
    printf("██  ██  ██ ██ ██  ██ ██ ██ ██  ██  ██ ██ ▄▄ ██    ██       ██   \n");
    printf("██      ██ ██ ██   ████ ██ ██      ██  ██████     ██       ██   \n");
    printf("                                          ▀▀\n");
    fflush(stdout);
}

int main(int argc, char * const argv[])
{
    int opt;

    printf_logo();

    config_init();

    while( (opt = getopt(argc, argv, "p:")) != -1 )
    {
        switch(opt)
        {
            case 'p':
                config->port = atoi(optarg);
                break;
            case '?':
                printf("格式错误\n");
                printf_help();
                break;
            default:
                printf("default, result=%c\n",opt);
                break;
        }
    }

    net_start();

    return 0;
}
