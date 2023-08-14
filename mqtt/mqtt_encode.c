#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mqtt_encode.h"
#include "mqtt.h"
#include "../log/log.h"

void ML_encode(int x, unsigned char * MSB, unsigned char * LSB){
    int encoded_byte = 0;

    *MSB = 0;
    *LSB = 0;

    do
    {
        encoded_byte = x % 128;
        x = x / 128;
        if(x > 0){
            encoded_byte = encoded_byte | 128;
            *MSB = encoded_byte;
        }
        *LSB = encoded_byte;
    } while (x > 0);
}

mqtt_string * string_encode(char * buff){
    mqtt_string * string = (mqtt_string *) malloc(sizeof(mqtt_string));
    memset(string, 0, sizeof(mqtt_string));

    if(buff == NULL){
        printf("error string len (NULL)\n");
        return string;
    }
    
    int x = strlen(buff);

    if(x > (128 * 128)){
        printf("error string len (more)\n");
        return string;
    }

    string->string_len = x;

    ML_encode(x, &(string->length_MSB), &(string->length_LSB));

    string->string = buff;

    return string;
}

char * const_packet_to_hex(struct mqtt_const_packet packet){
    char * buff = (char *) malloc(5);
    memset(buff, 0, 4);

    buff[0] = packet.const_header.control_packet_1 * 0x10 + packet.const_header.control_packet_2;
    buff[1] = packet.const_header.remaining_length;
    buff[2] = packet.variable_header.byte1;
    buff[3] = packet.variable_header.byte2;

    return buff;
}

char * conncet_packet_to_hex(struct connect_packet packet){
    char * buff = (char *) malloc(sizeof(char) * packet.connect_header.remaining_length + 3);
    char * flag = buff;
    memset(buff, 0, packet.connect_header.remaining_length + 2);

    *buff++ = packet.connect_header.control_packet_1 * 0x10 + packet.connect_header.control_packet_2;
    *buff++ = packet.connect_header.remaining_length;

    *buff++ = packet.variable_header.protocol_name->length_MSB;
    *buff++ = packet.variable_header.protocol_name->length_LSB;

    memcpy(buff, packet.variable_header.protocol_name->string, packet.variable_header.protocol_name->string_len);

    buff += packet.variable_header.protocol_name->string_len;

    *buff++ = packet.variable_header.protocol_level;

    *buff++ = packet.variable_header.connect_flags;

    *buff++ = packet.variable_header.keep_alive_MSB;
    *buff++ = packet.variable_header.keep_alive_LSB;

    *buff++ = packet.payload.client_id->length_MSB;
    *buff++ = packet.payload.client_id->length_LSB;

    if(packet.payload.client_id->string_len){
        memcpy(buff, packet.payload.client_id->string, packet.payload.client_id->string_len);
        buff += packet.payload.client_id->string_len;
    }

    if(packet.variable_header.connect_flags >> 2 & 1){
        *buff++ = packet.payload.will_topic->length_MSB;
        *buff++ = packet.payload.will_topic->length_LSB;

        memcpy(buff, packet.payload.will_topic->string, packet.payload.will_topic->string_len);
        buff += packet.payload.will_topic->string_len;

        *buff++ = packet.payload.will_message->length_MSB;
        *buff++ = packet.payload.will_message->length_LSB;

        memcpy(buff, packet.payload.will_message->string, packet.payload.will_message->string_len);
        buff += packet.payload.will_message->string_len;
    }

    if(packet.variable_header.connect_flags >> 6 & 1){
        *buff++ = packet.payload.user_name->length_MSB;
        *buff++ = packet.payload.user_name->length_LSB;

        memcpy(buff, packet.payload.user_name->string, packet.payload.user_name->string_len);
        buff += packet.payload.user_name->string_len;
    }

    if(packet.variable_header.connect_flags >> 7 & 1){
        *buff++ = packet.payload.password->length_MSB;
        *buff++ = packet.payload.password->length_LSB;
        memcpy(buff, packet.payload.password->string, packet.payload.password->string_len);
        buff += packet.payload.password->string_len;
    }

    printf_buff("connect packet", flag, packet.connect_header.remaining_length + 2);

    return flag;
}

char * mqtt_conncet_encode(unsigned char c_flag, int keep_alive
                            , char * client_id, char * will_topic, char * will_message
                            , char * user_name, char * password){
    struct connect_packet packet;

    packet.connect_header.control_packet_1 = 1;
    packet.connect_header.control_packet_2 = 0;

    packet.variable_header.protocol_name = string_encode("MQTT");

    packet.variable_header.protocol_level = 4;

    if(c_flag % 2){
        return NULL;
    }

    packet.variable_header.connect_flags = c_flag;

    if(keep_alive < 128 * 128)
        ML_encode(keep_alive, &(packet.variable_header.keep_alive_MSB), &(packet.variable_header.keep_alive_LSB));
    else{
        packet.variable_header.keep_alive_MSB = 0;
        packet.variable_header.keep_alive_LSB = 60;
    }

    packet.connect_header.remaining_length = 10;
    
    packet.payload.client_id = string_encode(client_id);

    packet.connect_header.remaining_length += packet.payload.client_id->string_len + 2;

    if(c_flag >> 2 & 1){
        packet.payload.will_topic = string_encode(will_topic);
        packet.payload.will_message = string_encode(will_message);
        packet.connect_header.remaining_length += packet.payload.will_topic->string_len + \
                                                  packet.payload.will_message->string_len +4;
    }

    if(c_flag >> 6 & 1){
        packet.payload.user_name = string_encode(user_name);
        packet.connect_header.remaining_length += packet.payload.user_name->string_len + 2;
    }

    if(c_flag >> 7 & 1){
        packet.payload.password = string_encode(password);
        packet.connect_header.remaining_length += packet.payload.password->string_len + 2;
    }

    return conncet_packet_to_hex(packet);
}

char * mqtt_connack_encode(int acknowledge_flag, int return_code){
    connack_packet connack_packet;

    connack_packet.const_header.control_packet_1 = 2;
    connack_packet.const_header.control_packet_2 = 0;
    connack_packet.const_header.remaining_length = 2;
    connack_packet.variable_header.byte1 = acknowledge_flag;
    connack_packet.variable_header.byte2 = return_code;

    return const_packet_to_hex(connack_packet);
}

char * publish_packet_to_hex(struct publish_packet packet){
    //TODO 这个只是qos0的，将他修改为qos1，qos2等可以的
    char * buff = (char *) malloc(sizeof(char) * (3 + packet.publish_header.remaining_length));
    char * flag = buff;
    memset(buff, 0, 2 + packet.publish_header.remaining_length);
    
    *buff++ = packet.publish_header.control_packet_1 * 0x10 + packet.publish_header.control_packet_2;
    *buff++ = packet.publish_header.remaining_length;
    *buff++ = packet.variable_header.topic_name->length_MSB;
    *buff++ = packet.variable_header.topic_name->length_LSB;
    
    memcpy(buff, packet.variable_header.topic_name->string, packet.variable_header.topic_name->string_len);

    buff += packet.variable_header.topic_name->string_len;

    //TODO 因为是qos0所以不设置报文标识

    memcpy(buff, packet.payload, strlen(packet.payload));

    return flag;
}

int mqtt_publish_encode(unsigned char * topic, unsigned char * payload, unsigned char * buff){
    struct publish_packet packet;
    packet.publish_header.control_packet_1 = 3;
    packet.publish_header.control_packet_2 = 0;
    packet.variable_header.topic_name = string_encode(topic);

    packet.payload = payload;
    printf("packet.payload:%s strlen:%d\n", packet.variable_header.topic_name->string, packet.variable_header.topic_name->string_len);

    packet.publish_header.remaining_length = packet.variable_header.topic_name->string_len + strlen(packet.payload) + 2;

    memcpy(buff, publish_packet_to_hex(packet), packet.publish_header.remaining_length + 2);

    return packet.publish_header.remaining_length + 2;
}

char * mqtt_publish_qos_encode(int control_type, int flag, int identifier_MSB, int identifier_LSB){
    puback_packet puback_packet;

    puback_packet.const_header.control_packet_1 = control_type;
    puback_packet.const_header.control_packet_2 = flag;
    puback_packet.const_header.remaining_length = 2;
    puback_packet.variable_header.byte1 = identifier_MSB;
    puback_packet.variable_header.byte2 = identifier_LSB;

    return const_packet_to_hex(puback_packet);
}

char * subscribe_packet_to_hex(struct subscribe_packet packet){
    char * buff = (char *) malloc(sizeof(char) * packet.subscribe_header.remaining_length + 3);
    char * flag = buff;
    memset(buff, 0, packet.subscribe_header.remaining_length + 2);

    *buff++ = packet.subscribe_header.control_packet_1 * 0x10 + packet.subscribe_header.control_packet_2;
    *buff++ = packet.subscribe_header.remaining_length;

    *buff++ = packet.variable_header.identifier_MSB;
    *buff++ = packet.variable_header.identifier_LSB;

    for(int i = 0; i < packet.topic_size; i++){
        *buff++ = packet.payload[i].topic_filter->length_MSB;
        *buff++ = packet.payload[i].topic_filter->length_LSB;

        strcpy(buff, packet.payload[i].topic_filter->string);

        buff += packet.payload[i].topic_filter->string_len;
        *buff++ = packet.payload[i].qos;
    }

    printf_buff("flag", flag, packet.subscribe_header.remaining_length + 2);

    return flag;
}

int mqtt_subscribe_encode(char ** topic, int * qos, int topic_size, int identifier, unsigned char * buff){
    struct subscribe_packet packet;
    packet.subscribe_header.control_packet_1 = 8;
    packet.subscribe_header.control_packet_2 = 2;
    
    ML_encode(identifier, &(packet.variable_header.identifier_MSB), &(packet.variable_header.identifier_LSB));

    packet.subscribe_header.remaining_length = 2;

    packet.payload = (struct subscribe_payload *) malloc(sizeof(struct subscribe_payload) * topic_size);
    packet.topic_size = topic_size;

    for (int i = 0; i < packet.topic_size; i++){
        packet.payload[i].topic_filter = string_encode(*(topic + i));
        packet.payload[i].qos = qos[i];
        packet.subscribe_header.remaining_length += packet.payload->topic_filter->string_len + 3;
    }

    memcpy(buff, subscribe_packet_to_hex(packet), packet.subscribe_header.remaining_length + 2);

    return packet.subscribe_header.remaining_length + 2;
}

char * suback_packet_to_hex(struct suback_packet packet){
    char * buff = (char *) malloc(4 + packet.return_code_size);
    memset(buff, 0, 4 + packet.return_code_size);

    buff[0] = packet.suback_header.control_packet_1 * 0x10 + packet.suback_header.control_packet_2;
    buff[1] = packet.suback_header.remaining_length;
    buff[2] = packet.variable_header.identifier_MSB;
    buff[3] = packet.variable_header.identifier_LSB;

    for(int i = 4; i < 4 + packet.return_code_size; i++){
        buff[i] = 0;
    }

    return buff;
}

char * mqtt_pingresp_encode(){
    char * buff = (char *) malloc(sizeof(char) * 3);
    memset(buff, 0, sizeof(char) * 3);

    buff[0] = 13 * 0x10 + 0;
    buff[1] = 0;

    return buff;
}

char * mqtt_suback_encode(int topic_size){
    struct suback_packet packet;

    packet.suback_header.control_packet_1 = 9;
    packet.suback_header.control_packet_2 = 0;

    //TODO 当接收不到回包的时候自加
    packet.variable_header.identifier_MSB = 0;
    packet.variable_header.identifier_LSB = 1;

    packet.return_codes = (unsigned char *) malloc(sizeof(unsigned char) * topic_size);
    memset(packet.return_codes, 0, sizeof(unsigned char) * topic_size);

    //TODO 当不满足条件的时候为0x80,并且配合qos
    for(int i = 0; i < topic_size; i++){
        packet.return_codes[i] = 0;
    }

    packet.return_code_size = topic_size;

    packet.suback_header.remaining_length = 2 + packet.return_code_size;

    return suback_packet_to_hex(packet);
}
