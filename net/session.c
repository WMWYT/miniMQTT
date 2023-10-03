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
void session_delete(struct session * s){
    char **p = NULL;

    if(utarray_front(s->topic)){
        while(p = (char **)utarray_next(s->topic, p)){
            session_topic_unsubscribe(*p, s->client_id);
        }

        if(utarray_front(s->topic) != NULL){
            utarray_free(s->topic);
        }
    }

    HASH_DELETE(hh1, session_sock, s);
    HASH_DELETE(hh2, session_client_id, s);

    if(s)
        free(s);
}

struct session * session_init(int s_sock, char * s_client_id, int clean_session){
    struct session * s;
    char * client_id;
    int tmp = 0;
    
    HASH_FIND(hh1, session_sock, &s_sock, sizeof(int), s);

    if (s){
        session_delete(s);
        return NULL;
    }

    if(s_client_id != NULL){
        HASH_FIND(hh2, session_client_id, s_client_id, strlen(s_client_id), s);
        if (s){
            //TODO 这里的clean session都为1，之后把他修改为0也可以使用
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

void session_add_will_topic(char * s_will_topic, struct session *s){
    if(s->will_topic == NULL){
        s->will_topic = (char *) malloc(sizeof(char) * (strlen(s_will_topic) + 1));
        memset(s->will_topic, 0, sizeof(char) * (strlen(s_will_topic) + 1));
    }

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
    char playload[10] = {0};
    char buff[255] = {0};
    char ** p = NULL;

    will_client_id = session_topic_search(s->will_topic);
    if(will_client_id != NULL){
        if(utarray_front(will_client_id) != NULL){
            buff_size = mqtt_publish_encode(s->will_topic, s->will_playload, buff);
            while((p = (char **) utarray_next(will_client_id, p))){
                HASH_FIND(hh2, session_client_id, *p, strlen(*p), will_s);
                write(will_s->sock, buff, buff_size);
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
void session_topic_subscribe(char * s_topic, char * s_client_id){
    intercept(s_topic, s_client_id);
}

void session_topic_unsubscribe(char * topic, char * client_id){
    delete_topic(topic, client_id);
}

void session_topic_delete_all(){
    if(root.client_id != NULL){
        utarray_clear(root.client_id);
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
    char **p = NULL;
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
    if(root.client_id != NULL){
        while ( (p=(char**)utarray_next(root.client_id,p))) {
            printf("%s ",*p);
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