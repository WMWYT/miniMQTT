#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "../../mqtt/mqtt.h"
#include "../../net/control.h"

void broker_control_info(char * control_type){
    char * type = "file";
    strcpy(control_type, type);
}

int connect_call_back(){

    return 4;
}

int broker_control_strat(void){
    int error = 0;

    error = control_register(connect_call_back, CONNECT);

    return error;
}