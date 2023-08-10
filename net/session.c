#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "session.h"
#include "filtering.h"

extern struct RootNode root;
struct session *session_sock;
struct session *session_client_id;

char * get_rand_str(int num){
    int randomData = 0;
    int file = open("/dev/urandom", O_RDONLY);
    char *str = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    char * s = (char *) malloc(sizeof(char) * num + 1);
    int i,lstr;
    char ss[2] = {0};
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

int session_init(int s_sock, char * s_client_id){
    struct session * s;
    char * client_id;
    int tmp = 0;
    
    HASH_FIND(hh1, session_sock, &s_sock, sizeof(int), s);

    if (s){
        session_delete(s);
        return -1;
    }

    if(s_client_id != NULL){
        HASH_FIND(hh2, session_client_id, s_client_id, strlen(s_client_id), s);
        if (s){
            //TODO 这里的clean session都为1，之后把他修改为0也可以使用
            tmp = s->sock;
        
            utarray_free(s->topic);
            HASH_DELETE(hh1, session_sock, s);
            HASH_DELETE(hh2, session_client_id, s);

            s = (struct session *) malloc(sizeof * s);
            memset(s, 0, sizeof *s);
            s->sock = s_sock;
            strcpy(s->client_id, s_client_id);
            utarray_new(s->topic, &ut_str_icd);

            HASH_ADD(hh1, session_sock, sock, sizeof(int), s);
            HASH_ADD(hh2, session_client_id, client_id, strlen(s->client_id), s);

            return tmp;
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

        HASH_ADD(hh1, session_sock, sock, sizeof(int), s);
        HASH_ADD(hh2, session_client_id, client_id, strlen(s->client_id), s);
    }

    return 0;
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
}

UT_array * session_topic_search(char * topic){
    return search(topic);
}

void session_topic_printf_all(){
    char **p = NULL;
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