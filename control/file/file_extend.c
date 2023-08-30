#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "../../mqtt/mqtt.h"
#include "../../net/control.h"

void broker_control_info(char * control_type){
    char * type = "file";
    strcpy(control_type, type);
}

int connect_call_back(void * packet){
    struct connect_packet * connect = (struct connect_packet *) packet;

    if(connect->payload.user_name == NULL || connect->payload.password == NULL){
        goto end;
    }

    if(!strcmp(connect->payload.user_name->string, "wmwyt") && !strcmp(connect->payload.password->string, "123")){
        return CONNECT_ACCEPTED;
    }

end:
    return CONNECT_ERROR_USER_OR_PASSWORD;
}

int broker_control_strat(void){
    int error = 0;

    error = control_register(connect_call_back, CONNECT);

    return error;
}