#ifndef MQTT_DECODE_H
#define MQTT_DECODE_H

union mqtt_packet * mqtt_pack_decode(unsigned char * buff, int * packet_len);

#endif