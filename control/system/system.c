#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "../../mqtt/mqtt.h"
#include "../../net/control.h"

void broker_control_info(char * control_type){
    char * type = "system";
    strcpy(control_type, type);
}

int system_call_back(void * system_date){
    struct system_info * info = (struct system_info *) system_date;
    int system_change = info->change;

    
}

int broker_control_strat(){
    int error = 0;

    error = control_register(system_call_back, SYSTEM);

    return error;
}