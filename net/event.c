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

void session_close(struct session *s){
    char **p = NULL;
    while(p = (char **) utarray_next(s->topic, p)){
        session_topic_unsubscribe(*p, s->client_id);
    }
    session_delete(s);
}

int event_handle(int * packet_len, char * buff, int fd){
    struct session * s;
    struct session_topic * st;

    int session_flag;
    //TODO 增加登陆功能,这里的qos>0只是能实现消息分发，没有qos>0的特性
    
    HASH_FIND(hh1, session_sock, &fd, sizeof(int), s);
    mqtt_packet = mqtt_pack_decode(buff, packet_len);

    if(mqtt_packet == NULL){
        if(s) session_close(s);
        return -1; // 小于0 有错误，要断开链接不发送
    }
    
    if(mqtt_packet->connect->connect_header.control_packet_1 == CONNECT){
        int error_code = mqtt_packet->connect->error_code;

        if(config->is_anonymously && error_code == CONNECT_ACCEPTED){
            error_code = control_connect(mqtt_packet->connect);
        }

        send(fd, mqtt_connack_encode(CONNACK, error_code), 4, 0);

        if(error_code == CONNECT_ACCEPTED){
            if((session_flag = session_init(fd, mqtt_packet->connect->payload.client_id->string)) >= 0){
                if(session_flag > 0){
                    return session_flag; //大于0 断开之前会话client_id相同的sock
                }

                return 0;
            }else{
                printf("Repeat connect\n");
                if(s) session_close(s);
                
                return -1;
            }
        }else{
            return -1;
        }
    }

    if(s == NULL){
        return -1; //未发送connect初始化
    }

    if(mqtt_packet->publish->publish_header.control_packet_1 == PUBLISH){
        UT_array * publish_client_id;
        char ** p = NULL;

        publish_client_id = session_topic_search(mqtt_packet->publish->variable_header.topic_name->string);

        if(publish_client_id != NULL){
            while((p = (char **) utarray_next(publish_client_id, p))){
                HASH_FIND(hh2, session_client_id, *p, strlen(*p), s);
                write(s->sock, buff, mqtt_packet->publish->publish_header.remaining_length + 2);
            }
        }

        if(mqtt_packet->publish->qos == 1){
            write(fd, mqtt_publish_qos_encode(PUBACK, 0, mqtt_packet->publish->variable_header.identifier_MSB, mqtt_packet->publish->variable_header.identifier_LSB), 4);
        }
        
        if(mqtt_packet->publish->qos == 2){
            //TODO 这里其实并未存储报文标识符
            write(fd, mqtt_publish_qos_encode(PUBREC, 0, mqtt_packet->publish->variable_header.identifier_MSB, mqtt_packet->publish->variable_header.identifier_LSB), 4);
        }
    }
    
    if(mqtt_packet->const_packet->const_header.control_packet_1 == PUBREC){
        printf("dfsdfsdds\n");
        write(fd, mqtt_publish_qos_encode(PUBREL, 2, mqtt_packet->const_packet->variable_header.byte1, mqtt_packet->const_packet->variable_header.byte2), 4);
    }

    if(mqtt_packet->const_packet->const_header.control_packet_1 == PUBREL || mqtt_packet->const_packet->const_header.control_packet_1 == PUBREC){
        //TODO 找到存储的publish消息并删除
        write(fd, mqtt_publish_qos_encode(PUBCOMP, 0, mqtt_packet->const_packet->variable_header.byte1, mqtt_packet->const_packet->variable_header.byte2), 4);
    }

    if(mqtt_packet->subscribe->subscribe_header.control_packet_1 == SUBSCRIBE){
        write(fd, mqtt_suback_encode(mqtt_packet->subscribe->topic_size), mqtt_packet->subscribe->topic_size + 4);
        for(int i = 0; i < mqtt_packet->subscribe->topic_size; i++){
            session_subscribe_topic(mqtt_packet->subscribe->payload[i].topic_filter->string, s);
            session_topic_subscribe(mqtt_packet->subscribe->payload[i].topic_filter->string, s->client_id);
        }
    }

    if(mqtt_packet->unsubscribe->unsubscribe_header.control_packet_1 == UNSUBSCRIBE){
        for(int i  = 0; i < mqtt_packet->unsubscribe->un_topic_size; i++){
            session_unsubscribe_topic(mqtt_packet->unsubscribe->payload[i]->string, s);
            session_topic_unsubscribe(mqtt_packet->unsubscribe->payload[i]->string, s->client_id);
        }
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