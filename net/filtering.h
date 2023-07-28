#ifndef FILTERING_H
#define FILTERING_H

#include "../utils/uthash/uthash.h"
#include "../utils/uthash/utarray.h"

struct h_node{
    char * data;
    
    UT_array * user_id;
    UT_array * leaf_nodes;
    UT_array * plus;
    UT_array * pound;
    UT_hash_handle hh;
};

struct b_node{
    char * data;

    UT_array * user_id;
    UT_array * leaf_nodes;
    UT_array * plus;
    UT_array * pound;
};

struct special_plus{
    UT_array * user_id;
    UT_array * leaf_nodes;
};

struct special_pound{
    UT_array * user_id;
};

void topic_init(char * topic, char * user_id);

#endif