#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
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
    char playload[10] = {0};
    char buff[255] = {0};
    char ** p = NULL;

    // version_client_id = session_topic_search("$SYS/broker/version");
    // time_client_id = session_topic_search("$SYS/broker/time");

    // switch (system_change){
    //     case 0:
    //         active_client_id = session_topic_search("$SYS/broker/active");
    //         if(active_client_id != NULL){
    //             sprintf(playload, "%d", info->active);
    //             buff_size = mqtt_publish_encode("$SYS/broker/active", playload, buff);
    //             while((p = (char **) utarray_next(active_client_id, p))){
    //                 HASH_FIND(hh2, session_client_id, *p, strlen(*p), s);
    //                 write(s->sock, buff, buff_size);
    //             }
    //         }
    //         break;
    //     default:
    //         break;
    // }
}

int broker_control_strat(){
    int error = 0;

    error = control_register(system_call_back, SYSTEM);

    return error;
}