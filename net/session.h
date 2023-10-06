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
    int clean_session;
    int will_qos;
    char * will_topic;
    char * will_playload;
    UT_array * topic;

    UT_hash_handle hh1;
    UT_hash_handle hh2;
};

struct session_publish{
    char client_id[64];

    char * topic;
    char * playload;

    UT_hash_handle hh;
};

static char session_packet_identifier[65536];

struct session * session_init(int s_sock, char * s_client_id, int clean_session);
void session_add_will_topic(char * s_will_topic, int qos, struct session *s);
void session_add_will_playload(char * s_will_playload, struct session * s);
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

void session_publish_add(char * client_id, int topic_len, char * topic, int playload_len, char * playload);
void session_publish_delete();
void session_publish_delete_all();
void session_publish_printf_all();

#endif