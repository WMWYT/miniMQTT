#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "session.h"
#include "filtering.h"
#include "control.h"
#include "../mqtt/mqtt_encode.h"

extern struct RootNode root;
extern struct SYSNode sys;
struct session_info * session_info;
struct session *session_sock;
struct session *session_client_id;
struct session_publish * session_publish;

char * get_rand_str(int num){
    int randomData = 0;
    int file = open("/dev/urandom", O_RDONLY);
    char *str = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    char * s = (char *) malloc(sizeof(char) * num + 1);
    int i,lstr;
    char ss[2] = {0};

    memset(s, 0, sizeof(char) * num + 1);
    lstr = strlen(str);

    if (file == -1) {
        perror("Failed to open /dev/urandom");
        exit(EXIT_FAILURE);
    }
    
    if (read(file, &randomData, sizeof(randomData)) == -1) {
        perror("Failed to read /dev/urandom");
        close(file);
        exit(EXIT_FAILURE);
    }

    close(file);

    srand(abs(randomData));
    for(i = 1; i <= num; i++){
        sprintf(ss,"%c",str[(rand() % lstr)]);
        strcat(s,ss);
    }

    return s;
}

/**************session***************/
struct session * session_init(int s_sock, char * s_client_id, int clean_session){
    struct session * s = NULL;
    char * client_id;
    int tmp = 0;

    if(s_client_id != NULL){
        HASH_FIND(hh2, session_client_id, s_client_id, strlen(s_client_id), s);
        if (s){
            tmp = s->sock;
            utarray_free(s->topic);
            if(s->will_topic) free(s->will_topic);
            if(s->will_topic) free(s->will_playload);
            HASH_DELETE(hh1, session_sock, s);
            HASH_DELETE(hh2, session_client_id, s);

            s = (struct session *) malloc(sizeof * s);
            memset(s, 0, sizeof *s);
            s->sock = tmp;
            strcpy(s->client_id, s_client_id);
            utarray_new(s->topic, &ut_str_icd);
            s->clean_session = clean_session;

            HASH_ADD(hh1, session_sock, sock, sizeof(int), s);
            HASH_ADD(hh2, session_client_id, client_id, strlen(s->client_id), s);

            return s;
        }
    } else {
        s_client_id = (char *) malloc(sizeof(char) * 9);
        strcpy(s_client_id, get_rand_str(8));
    }

    if(s == NULL){
        s = (struct session *) malloc(sizeof *s);
        memset(s, 0, sizeof *s);
        s->sock = s_sock;
        strcpy(s->client_id, s_client_id);
        utarray_new(s->topic, &ut_str_icd);
        s->clean_session = clean_session;

        HASH_ADD(hh1, session_sock, sock, sizeof(int), s);
        HASH_ADD(hh2, session_client_id, client_id, strlen(s->client_id), s);
    }

    int a = 1;
    system_info_update(&a, 0);

    return s;
}

void session_delete(struct session * s){
    if(s->clean_session){
        char **p = NULL;

        //当删除会话的时候从session中找出topic并逐个删掉
        if(utarray_front(s->topic)){
            while(p = (char **)utarray_next(s->topic, p)){
                session_topic_unsubscribe(*p, s->client_id);
            }
        }

        HASH_DELETE(hh1, session_sock, s);
        HASH_DELETE(hh2, session_client_id, s);

        if(s)
            free(s);
    }
}

void session_add_will_topic(char * s_will_topic, int qos, struct session *s){
    if(s->will_topic == NULL){
        s->will_topic = (char *) malloc(sizeof(char) * (strlen(s_will_topic) + 1));
        memset(s->will_topic, 0, sizeof(char) * (strlen(s_will_topic) + 1));
    }

    s->will_qos = qos;
    printf("session_add_will_topic:%d\n", s->will_qos);
    memmove(s->will_topic, s_will_topic, strlen(s_will_topic));
}

void session_add_will_playload(char * s_will_playload, struct session * s){
    if(s->will_playload == NULL){
        s->will_playload = (char *)malloc(sizeof(char) * (strlen(s_will_playload) + 1));
        memset(s->will_playload, 0, sizeof(char) * (strlen(s_will_playload) + 1));
    }

    memmove(s->will_playload, s_will_playload, strlen(s_will_playload));
}

void session_subscribe_topic(char * s_topic, struct session *s){
    if(utarray_find(s->topic, &s_topic, strsort) == NULL){
        utarray_push_back(s->topic, &s_topic);
    }
    
    utarray_sort(s->topic, strsort);
}

void session_unsubscribe_topic(char * s_topic, struct session * s){
    char **first, **find;
    long int pos = 0;
    
    if((find = utarray_find(s->topic, &s_topic, strsort)) != NULL){
        first = utarray_front(s->topic);
        pos = find - first;

        utarray_erase(s->topic, pos, 1);
    }

    utarray_sort(s->topic, strsort);
}

void session_printf_all(){
    struct session *current;
    struct session *tmp;
    char **p = NULL;

    HASH_ITER(hh1, session_sock, current, tmp) {
        printf("sock %d: %s subscribe topic -- ", current->sock, current->client_id);
        while((p = (char **) utarray_next(current->topic, p)))
           printf("%s ", *p);
        printf("\n");

        printf("will topic -- %s playload -- %s\n", current->will_topic, current->will_playload);
    }
}

void session_delete_all(){
    struct session *current;
    struct session *tmp;

    HASH_ITER(hh1, session_sock, current, tmp) {
        HASH_DELETE(hh1, session_sock, current);
        if(current)
            free(current);
    }

    HASH_ITER(hh2, session_client_id, current, tmp){
        HASH_DELETE(hh2, session_client_id, current);
        if(current)
            free(current);
    }
}

void publish_will_message(struct session * s){
    struct session * will_s;
    UT_array * will_client_id;
    int buff_size = 0;
    int qos = s->will_qos;
    char buff[65535] = {0};
    ChilderNode * p = NULL;

    will_client_id = session_topic_search(s->will_topic);
    if(will_client_id != NULL){
        if(utarray_front(will_client_id) != NULL){
            while((p = (ChilderNode *) utarray_next(will_client_id, p))){
                HASH_FIND(hh2, session_client_id, p->client_id, strlen(p->client_id), will_s);
                
                if(s->will_qos > p->max_qos)
                    qos = p->max_qos;

                buff_size = mqtt_publish_encode(s->will_topic, qos,s->will_playload, buff);
                write(will_s->sock, buff, buff_size);
                memset(buff, 0, buff_size);
            }
        }
    }
}

void session_close(struct session *s){
    int a = -1;

    publish_will_message(s);
    session_delete(s);
    system_info_update(&a, 0);
}

/*******************************topic************************************/
void session_topic_subscribe(char * s_topic, int max_qos, char * s_client_id){
    intercept(s_topic, max_qos, s_client_id);
}

void session_topic_unsubscribe(char * topic, char * client_id){
    delete_topic(topic, client_id);
}

void session_topic_delete_all(){
    if(root.childer_node != NULL){
        utarray_clear(root.childer_node);
    }

    if(root.plus_children != NULL){
        delete_all(root.plus_children);
    }

    if(root.children != NULL){
        delete_all(root.children);
    }

    if(sys.children != NULL){
        delete_all(sys.children);
    }

    if(sys.plus_children != NULL){
        delete_all(sys.plus_children);
    }
}

UT_array * session_topic_search(char * topic){
    if(topic != NULL)
        return search(topic);
    
    return NULL;
}

void session_topic_printf_all(){
    ChilderNode *p = NULL;
    printf("-------------------system-------------\n");
    printf("+\n");

    free(p);
    p = NULL;

    if(sys.plus_children != NULL){
        printf_all(sys.plus_children);
    }

    printf("\nnormal\n");

    printf_all(sys.children);


    printf("-------------------root-------------\n");
    printf("#\n");
    if(root.childer_node != NULL){
        while ( (p=(ChilderNode *)utarray_next(root.childer_node,p))) {
            printf("client_id:%s ", p->client_id);
            printf("max_qos:%d ", p->max_qos);
        }
    }

    printf("\n+\n");

    free(p);
    p = NULL;

    if(root.plus_children != NULL){
        printf_all(root.plus_children);
    }

    printf("\nnormal\n");

    printf_all(root.children);

    printf("____________________________________\n");
}

/*****************sessiong publish*****************/
void session_publish_add(char * client_id, int topic_len, char * topic, int playload_len, char * playload){
    struct session_publish * s;

    s = (struct session_publish *) malloc(sizeof * s);
    memset(s, 0, sizeof * s);
    strcpy(s->client_id, client_id);

    s->topic = (char *) malloc(sizeof(char) * topic_len);
    memset(s->topic, 0, topic_len);
    strcpy(s->topic, topic);

    s->playload = (char *) malloc(sizeof(char) * playload_len);
    memset(s->playload, 0, playload_len);
    strcpy(s->playload, playload);

    HASH_ADD_STR(session_publish, client_id, s);
}

void session_publish_delete(char * client_id){
    struct session_publish * s;

    HASH_FIND_STR(session_publish, client_id, s);

    HASH_DEL(session_publish, s);
    free(s);
}

void session_publish_delete_all(){
    struct session_publish *current;
    struct session_publish *tmp;

    HASH_ITER(hh, session_publish, current, tmp) {
      HASH_DEL(session_publish, current);
    }
}

void session_publish_printf_all(){
    struct session_publish *current;
    struct session_publish *tmp;
    char **p = NULL;

    HASH_ITER(hh, session_publish, current, tmp){
        printf("publish client id %s: topic %s playload %s\n", current->client_id, current->topic, current->playload);
    }
}