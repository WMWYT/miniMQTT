#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include "event.h"
#include "session.h"
#include "control.h"
#include "../mqtt/mqtt_decode.h"
#include "../mqtt/mqtt_encode.h"
#include "../mqtt/mqtt.h"

union mqtt_packet * mqtt_packet;
extern struct session * session_sock;
extern struct session * session_client_id;

int event_handle(int * packet_len, char * buff, int fd){
    struct session * s;
    struct session_topic * st;
    struct session * session_flag;
    
    //TODO 这里的qos>0只是能实现消息分发，没有qos>0的特性
    HASH_FIND(hh1, session_sock, &fd, sizeof(int), s);
    mqtt_packet = mqtt_pack_decode(buff, packet_len);

    if(mqtt_packet == NULL){
        if(s) session_close(s);
        return -1; // 小于0 有错误，要断开链接不发送
    }
    
    if(mqtt_packet->connect->connect_header.control_packet_1 == CONNECT){
        printf("packet connect\n");
        int error_code = mqtt_packet->connect->error_code;

        if(config->is_anonymously && error_code == CONNECT_ACCEPTED){
            error_code = control_connect(mqtt_packet->connect);
        }

        if(error_code == CONNECT_ACCEPTED){
            session_flag = session_init(fd, mqtt_packet->connect->payload.client_id->string, (mqtt_packet->connect->variable_header.connect_flags >> 1));

            if(mqtt_packet->connect->variable_header.connect_flags >> 2 & 1){
                session_add_will_topic(mqtt_packet->connect->payload.will_topic->string, \
                (mqtt_packet->connect->variable_header.connect_flags >> 4 * 2 + mqtt_packet->connect->variable_header.connect_flags >> 4 * 1), \
                session_flag);
                session_add_will_playload(mqtt_packet->connect->payload.will_playload->string, session_flag);
            }

            send(fd, mqtt_connack_encode(!(mqtt_packet->connect->variable_header.connect_flags >> 1), \
                                            CONNECT_ACCEPTED), \
                    4, 0);

            //TODO 如果这个的clean session为0就推送之前的消息
            if(!session_flag->clean_session){
                
            }

            if(session_flag->sock != fd){
                int tmp = session_flag->sock;
                session_flag->sock = fd;
                return tmp; //大于0 断开之前会话client_id相同的sock
            }

            return 0;
        }else{
            send(fd, mqtt_connack_encode(0, error_code), 4, 0);
        }

        return -1;
    }

    if(s == NULL){
        return -1; //未发送connect初始化
    }

    if(mqtt_packet->publish->publish_header.control_packet_1 == PUBLISH){
        printf("packet publish\n");
        UT_array * publish_client_id;
        char ** p = NULL;

        publish_client_id = session_topic_search(mqtt_packet->publish->variable_header.topic_name->string);

        printf("dup: %d qos: %d, retain: %d\n", mqtt_packet->publish->dup, mqtt_packet->publish->qos, mqtt_packet->publish->retain);

        if(publish_client_id != NULL){
            while((p = (char **) utarray_next(publish_client_id, p))){
                HASH_FIND(hh2, session_client_id, *p, strlen(*p), s);

                //存储消息
                // if(){
                //     session_publish_add(*p, mqtt_packet->publish->variable_header.topic_name->string_len, \
                //                         mqtt_packet->publish->variable_header.topic_name->string, \
                //                         mqtt_packet->publish->playload_len, mqtt_packet->publish->payload);
                //     session_publish_printf_all();
                // }

                //write(s->sock, mqtt_publish_encode(), );
                write(s->sock, buff, mqtt_packet->publish->publish_header.remaining_length + 2);
            }
        }

        //TODO 这里在服务器给客户端推送消息的时候没有考虑客户端订阅时所允许的最大qos
        //TODO 这里其实并未实现clean session为0时的信息存储
        //TODO 没有实现qos1，2的全功能

        if(mqtt_packet->publish->qos == 1){
            write(fd, mqtt_publish_qos_encode(PUBACK, 0, mqtt_packet->publish->variable_header.identifier_MSB, mqtt_packet->publish->variable_header.identifier_LSB), 4);
        }

        if(mqtt_packet->publish->qos == 2){
            write(fd, mqtt_publish_qos_encode(PUBREC, 0, mqtt_packet->publish->variable_header.identifier_MSB, mqtt_packet->publish->variable_header.identifier_LSB), 4);
        }
    }

    //QOS1
    if(mqtt_packet->const_packet->const_header.control_packet_1 == PUBACK){
        printf("puback packet\n");
        //TODO 丢弃消息
    }

    //QOS2
    if(mqtt_packet->const_packet->const_header.control_packet_1 == PUBREC){
        printf("pubrec packet\n");
        //TODO 丢弃消息
        write(fd, mqtt_publish_qos_encode(PUBREL, 2, mqtt_packet->const_packet->variable_header.byte1, mqtt_packet->const_packet->variable_header.byte2), 4);
    }

    if(mqtt_packet->const_packet->const_header.control_packet_1 == PUBREL){
        printf("pubrel packet\n");
        //TODO 丢弃报文标识符
        write(fd, mqtt_publish_qos_encode(PUBCOMP, 0, mqtt_packet->const_packet->variable_header.byte1, mqtt_packet->const_packet->variable_header.byte2), 4);
    }

    if(mqtt_packet->const_packet->const_header.control_packet_1 == PUBCOMP){
        printf("pubcomp packet\n");
        //TODO 丢弃状态
    }

    if(mqtt_packet->subscribe->subscribe_header.control_packet_1 == SUBSCRIBE){
        printf("packet subscribe\n");
        int * return_code = NULL;

        if(config->is_anonymously){
            return_code = control_subscribe(mqtt_packet->subscribe);
            for(int i = 0; i < mqtt_packet->subscribe->topic_size; i++){
                printf("subscribe code: %d\n", return_code[i]);
            }
        }

        write(fd, mqtt_suback_encode(mqtt_packet->subscribe->variable_header.identifier_MSB, \
                                    mqtt_packet->subscribe->variable_header.identifier_LSB, \
                                    mqtt_packet->subscribe->topic_size, return_code), \
            mqtt_packet->subscribe->topic_size + 4);

        for(int i = 0; i < mqtt_packet->subscribe->topic_size; i++){
            if(return_code == NULL || return_code[i] != 0x80){
                session_subscribe_topic(mqtt_packet->subscribe->payload[i].topic_filter->string, s);
                session_topic_subscribe(mqtt_packet->subscribe->payload[i].topic_filter->string, s->client_id);
            }
        }
    }

    if(mqtt_packet->unsubscribe->unsubscribe_header.control_packet_1 == UNSUBSCRIBE){
        printf("packet unsubscribe\n");
        for(int i  = 0; i < mqtt_packet->unsubscribe->un_topic_size; i++){
            session_unsubscribe_topic(mqtt_packet->unsubscribe->payload[i]->string, s);
            session_topic_unsubscribe(mqtt_packet->unsubscribe->payload[i]->string, s->client_id);
        }

        write(fd, mqtt_unsuback_encode(mqtt_packet->unsubscribe->variable_header.identifier_MSB,mqtt_packet->unsubscribe->variable_header.identifier_LSB), 4);
    }

    if(mqtt_packet->pingreq->pingreq_header.control_packet_1 == PINGREQ){
        write(fd, mqtt_pingresp_encode(), 2);
    }

    if(mqtt_packet->disconnect->disconnect_header.control_packet_1 == DISCONNECT){
        //TODO 这里只是clean session 为1清除会话的时候，之后将为0的放上去
        session_close(s);
        return -1;// 小于0 客户端断开链接， 要断开链接不发送
    }

    free(mqtt_packet);

    return 0; // 等于0 没问题
}