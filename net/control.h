#ifndef CONTROL_H
#define CONTROL_H

#include "../mqtt/mqtt.h"

int control_register(int (*call_back)(void *), int packet_type);

int control_connect(struct connect_packet * connect);

#endif