#ifndef CONTROL_H
#define CONTROL_H

#include "../mqtt/mqtt.h"

enum other_type{
    SYSTEM
};

struct system_info{
    char * version;
    char * time;
    int active;

    int change;
};

void system_info_init();

void system_info_update(void * info, int change);

int control_register(int (*call_back)(void *), int type);

int control_connect(struct connect_packet * connect);

#endif