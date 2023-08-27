#ifndef SESSION_H
#define SESSION_H

#include "../mqtt/mqtt.h"
#include "../utils/uthash/utarray.h"
#include "../utils/uthash/uthash.h"
#include "../utils/uthash/utlist.h"

struct session_info{
    int active;
};

struct session {
    int sock;
    char client_id[64];
    
    //info
    UT_array * topic;

    UT_hash_handle hh1;
    UT_hash_handle hh2;
};

void session_info_init();
int session_info_acitve_updata(int num);
void session_info_delete();

int session_init(int s_sock, char * s_client_id);
void session_subscribe_topic(char * s_topic, struct session *s);
void session_unsubscribe_topic(char * s_topic, struct session * s);
void session_printf_all();
void session_delete(struct session * s);
void session_delete_all();
void session_close(struct session *s);

void session_topic_subscribe(char * s_topic, char * s_client_id);
void session_topic_unsubscribe(char * topic, char * client_id);
void session_topic_delete_all();
UT_array * session_topic_search(char * topic);
void session_topic_printf_all();

#endif