#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include "../../net/filtering.h"
#include "../../utils/uthash/utarray.h"
#include "../../mqtt/mqtt.h"
#include "../../mqtt/mqtt_encode.h"
#include "../../net/control.h"
#include "../../net/session.h"

extern struct session * session_client_id;

void broker_control_info(char * control_type){
    char * type = "system";
    strcpy(control_type, type);
}

int system_call_back(void * system_date){
    UT_array * version_client_id;
    UT_array * time_client_id;
    UT_array * active_client_id;
    struct session * s;
    struct system_info * info = (struct system_info *) system_date;
    int system_change = info->change;
    int buff_size = 0;
    char payload[10] = {0};
    char buff[255] = {0};
    ChilderNode * p = NULL;

    // version_client_id = session_topic_search("$SYS/broker/version");
    // time_client_id = session_topic_search("$SYS/broker/time");

    switch (system_change){
        case 0:
            active_client_id = session_topic_search("$SYS/broker/active");
            if(utarray_front(active_client_id) != NULL){
                sprintf(payload, "%d", info->active);
                while((p = (ChilderNode *) utarray_next(active_client_id, p))){
                    printf("system_call_back:%s\n", p->client_id);
                    buff_size = mqtt_publish_encode_qos_0("$SYS/broker/active", payload, buff);
                    HASH_FIND(hh2, session_client_id, p->client_id, strlen(p->client_id), s);
                    memset(buff, 0, buff_size);
                    write(s->sock, buff, buff_size);
                }
            }
            break;
        default:
            break;
    }
}

int broker_control_strat(){
    int error = 0;

    error = control_register(system_call_back, SYSTEM);

    return error;
}