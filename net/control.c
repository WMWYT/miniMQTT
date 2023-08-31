#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>
#include "control.h"

void * control_lib;
struct system_info * system_info;
int (*broker_control_strat)();
static int (*connect_call_back)(void *);
static int (*system_call_back)(void *);
static int (*subscribe_call_back)(void *);

int control_init(const char * dl_dir, char * type){
    void (*broker_control_info)(char *);
    char tmp_type[16] = {0};
    char * error;

    control_lib = dlopen(dl_dir, RTLD_LAZY);

    if(control_lib == NULL){
        printf("dlopen:%s\n", dlerror());
        return -1;
    }

    (void) dlerror();

    *(void **) (&broker_control_info) = (void *)dlsym(control_lib, "broker_control_info");

    error = dlerror();
    if(error != NULL){
        printf("dlsym:%s\n", error);
        return -1;
    }

    if(type != NULL){
        broker_control_info(tmp_type);
        if(strcmp(tmp_type, type)){
            printf("error type not equal.\n");
            return -1;
        }
    }else{
        printf("error tpye is NULL.\n");
        return -1;
    }

    (void) dlerror();

    *(int **) (&broker_control_strat) = (int *)dlsym(control_lib, "broker_control_strat");

    error = dlerror();
    if(error != NULL){
        printf("dlsym:%s\n", error);
        return -1;
    }

    return broker_control_strat();
}

int control_register(int (*call_back)(void *), int type){//TODO 将其他包传入回调函数
    switch (type){
        case SYSTEM:
            system_call_back = call_back;
            break;
        case CONNECT:
            connect_call_back = call_back;
            break;
        case SUBSCRIBE:
            subscribe_call_back = call_back;
            break;
        default:
            return -1;
    }

    return 0;
}

void system_info_init(){
    static struct system_info info;
    info.version = "0.0.1";
    info.time = "8.27";
    info.active = 0;

    system_info = &info;
}

int control_system(struct system_info * info){
    return system_call_back(info);
}

void system_info_update(void * info, int change){
    switch (change){
        case 0:
            system_info->active += *(int *)info;
            system_info->change = change;

            if(system_info->active < 0){
                printf("error active num\n");
                return;
            }
            break;
        default:
            printf("not have this change [%d]\n", change);
            return;
    }

    control_system(system_info);
}

void session_info_delete(){
    if(system_info) free(system_info);
}

int control_connect(struct connect_packet * connect){
    return connect_call_back(connect);
}

int control_subscribe(struct subscribe_packet * subscribe){
    return subscribe_call_back(subscribe);
}

int control_destroyed(){
    if(broker_control_strat) free(broker_control_strat);
    if(connect_call_back) free(connect_call_back);
    if(subscribe_call_back) free(subscribe_call_back);
    return dlclose(control_lib);
}