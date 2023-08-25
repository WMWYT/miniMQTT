#ifndef FILTERING_H
#define FILTERING_H

#include "../utils/uthash/uthash.h"
#include "../utils/uthash/utarray.h"

struct TrieNode{
    char * key;
    struct TrieNode * children;
    struct TrieNode * plus_children;

    struct{
        UT_array * client_id;
    }PoundNode;

    UT_array * client_id;
    UT_hash_handle hh;
};

struct RootNode{
    struct TrieNode * children;
    struct TrieNode * plus_children;
    UT_array * client_id;
};

//TODO 添加以$为开头的主题

int intsort(const void *a, const void *b);
int strsort(const void *_a, const void *_b);

void intercept(char * key, char * client_id);
UT_array * search(char * key);
void delete_topic(char * key, char * client_id);
void delete_all(struct TrieNode * node);
void printf_all(struct TrieNode * s_root);

#endif