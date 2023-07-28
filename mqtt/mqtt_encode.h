#ifndef MQTT_ENCODE_H
#define MQTT_ENCODE_H

char * mqtt_conncet_encode(unsigned char c_flag, int keep_alive \
                            , char * client_id, char * will_topic, char * will_message \
                            , char * user_name, char * password);

char * mqtt_connack_encode(int acknowledge_flag, int return_code);

int mqtt_publish_encode(unsigned char * topic, unsigned char * payload, unsigned char * buff);

int mqtt_subscribe_encode(char ** topic, int * qos, int topic_size, int identifier, unsigned char * buff);

char * mqtt_pingresp_encode();

char * mqtt_suback_encode(int topic_size);

#endif