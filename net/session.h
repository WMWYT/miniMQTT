#ifndef SESSION_H
#define SESSION_H

#include "../mqtt/mqtt.h"
#include "../utils/uthash/utarray.h"
#include "../utils/uthash/uthash.h"
#include "../utils/uthash/utlist.h"

struct session {
    int sock;
    char client_id[64];
    
    //info
    UT_array * topic;

    UT_hash_handle hh1;
    UT_hash_handle hh2;
};

struct session_topic{
    char topic[64];
    UT_array * client_id;
    UT_hash_handle hh;
};

int session_init(int s_sock, char * s_client_id);
void session_subscribe_topic(char * s_topic, struct session *s);
void session_unsubscribe_topic(char * s_topic, struct session * s);
void session_printf_all();
void session_delete(struct session * s);
void session_delete_all();

void session_topic_subscribe(char * s_topic, char * s_client_id);
void session_topic_unsubscribe(char * topic, char * client_id);
void session_topic_delete_all();
void session_topic_printf_all();

#endif