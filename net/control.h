#ifndef CONTROL_H
#define CONTROL_H

#include "../mqtt/mqtt.h"

int control_init(const char * dl_dir, char * type);
int control_register(int (*call_back)(), int packet_type);
int control_destroyed();

int control_connect(struct connect_packet * connect);

#endif