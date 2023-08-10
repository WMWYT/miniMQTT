#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>
#include "filtering.h"

struct RootNode root;
UT_icd node_icd = {sizeof(struct TrieNode), NULL, NULL, NULL};

int intsort(const void *a, const void *b){
    int _a = *(const int *)a;
    int _b = *(const int *)b;
    return (_a < _b) ? -1 : (_a > _b);
}

int strsort(const void *_a, const void *_b){
    const char *a = *(const char* const *)_a;
    const char *b = *(const char* const *)_b;
    return strcmp(a,b);
}

void deduplication(UT_array * array){
    char ** p = NULL;
    char ** tmp = NULL;
    utarray_sort(array, strsort);

    for(int i = 0; (p=(char**)utarray_next(array,tmp)); i++) {
        if(tmp && !strcmp(*p, *tmp)){
            utarray_erase(array, i, 1);
        }else{
            tmp = p;
        }
    }
}

struct TrieNode * initNode(char * key){
    struct TrieNode *pCrawl;

    HASH_FIND_STR(root.children, key, pCrawl);

    if(pCrawl == NULL){
        pCrawl = (struct TrieNode *) malloc(sizeof(struct TrieNode));
        memset(pCrawl, 0, sizeof * pCrawl);
        pCrawl->key = key;
        pCrawl->client_id = NULL;
        HASH_ADD_STR(root.children, key, pCrawl);
    }

    return pCrawl;
}

struct TrieNode * initPlusNode(){
    struct TrieNode *pCrawl;
    HASH_FIND_STR(root.plus_children, "+", pCrawl);

    if(pCrawl == NULL){
        pCrawl = (struct TrieNode *) malloc(sizeof(struct TrieNode));
        memset(pCrawl, 0, sizeof * pCrawl);
        pCrawl->key = "+";
        pCrawl->client_id = NULL;
        HASH_ADD_STR(root.plus_children, key, pCrawl);
    }

    return pCrawl;
}

struct TrieNode * insert(struct TrieNode * s_root, char * key){
    struct TrieNode *tmpCrawl;
    HASH_FIND_STR(s_root->children, key, tmpCrawl);

    if(tmpCrawl == NULL){
        tmpCrawl = (struct TrieNode *) malloc(sizeof(struct TrieNode));
        memset(tmpCrawl, 0, sizeof * tmpCrawl);
        tmpCrawl->key = key;
        tmpCrawl->client_id = NULL;
        HASH_ADD_STR(s_root->children, key, tmpCrawl);
    }

    return tmpCrawl;
}

struct TrieNode * insertPlus(struct TrieNode * s_root, char * key){
    struct TrieNode *tmpCrawl;
    HASH_FIND_STR(s_root->plus_children, key, tmpCrawl);

    if(tmpCrawl == NULL){
        tmpCrawl = (struct TrieNode *) malloc(sizeof(struct TrieNode));
        memset(tmpCrawl, 0, sizeof * tmpCrawl);
        tmpCrawl->key = key;
        tmpCrawl->client_id = NULL;
        HASH_ADD_STR(s_root->plus_children, key, tmpCrawl);
    }

    return tmpCrawl;
}

void intercept(char * key, char * client_id){
    struct TrieNode *pCrawl;
    char * tmp_str;
    int tmp_int = 0;
    int i = 0;

    if(key[0] == '#'){
        if(root.client_id == NULL)
            utarray_new(root.client_id, &ut_str_icd);

        utarray_push_back(root.client_id, &client_id);
        deduplication(root.client_id);
        return;
    }else if(key[0] == '+'){
        pCrawl = initPlusNode();
        tmp_int = i = 1;
    }else if(key[0] == '/'){
        pCrawl = initNode("/");
        tmp_int = i = 1;
    }else{
        for(i = 0; key[i] != '\0' && key[i] != '/'; i++);
        tmp_str = (char *) malloc(sizeof(char) * (i));
        memset(tmp_str, 0, sizeof(char) * (i));
        strncpy(tmp_str, &key[0], i);
        tmp_str[i] = '\0';
        pCrawl = initNode(tmp_str);
    }

    for(; key[i] != '\0'; i++){
        if(key[i] == '/'){
            if(key[i + 1] == '/' || i == 0 || key[i + 1] == '\0'){
                pCrawl = insert(pCrawl, "/");
            }

            tmp_int = i + 1;
        }else{
            for(i; key[i + 1] != '\0' && key[i + 1] != '/'; i++);
            
            tmp_str = (char *) malloc(sizeof(char) * (i - tmp_int + 1));
            memset(tmp_str, 0, sizeof(char) * (i - tmp_int + 1));
            strncpy(tmp_str, &key[tmp_int], i - tmp_int + 1);

            if(!strcmp(tmp_str, "#")){
                goto pound;
            }else if(!strcmp(tmp_str, "+")){
                pCrawl = insertPlus(pCrawl, "+");
            }else{
                pCrawl = insert(pCrawl, tmp_str);
            }
        }
    }

    goto end;

pound:
    if(pCrawl->PoundNode.client_id == NULL)
        utarray_new(pCrawl->PoundNode.client_id, &ut_str_icd);

    utarray_push_back(pCrawl->PoundNode.client_id, &client_id);
    deduplication(pCrawl->PoundNode.client_id);

end:
    if(pCrawl->client_id == NULL)
        utarray_new(pCrawl->client_id, &ut_str_icd);
    
    utarray_push_back(pCrawl->client_id, &client_id);
    deduplication(pCrawl->client_id);
}

UT_array * array_hand(UT_array * array, char * key){
    struct TrieNode * p = NULL;
    struct TrieNode * tmpCrawl = NULL;
    UT_array * pound_array = NULL;
    UT_array * ans_array = NULL;

    utarray_new(pound_array, &ut_str_icd);
    utarray_new(ans_array, &node_icd);

    for(int i = 0; (p = (struct TrieNode *) utarray_next(array, p)); i++){
        HASH_FIND_STR(p->children, key, tmpCrawl);

        if(p->plus_children != NULL)
            utarray_push_back(ans_array, p->plus_children);

        if(p->PoundNode.client_id != NULL)
            utarray_concat(pound_array, p->PoundNode.client_id);

        if(tmpCrawl != NULL){
            utarray_push_back(ans_array, tmpCrawl);
        }
    }

    utarray_clear(array);
    utarray_concat(array, ans_array);

    return pound_array;
}

UT_array * search(char * key){
    UT_array * pCrawl = NULL;
    UT_array * tmp_array = NULL;
    UT_array * test_array = NULL;

    struct TrieNode *tmpCrawl = NULL;
    struct TrieNode * p = NULL;
    
    char ** test = NULL;
    char * tmp_str = NULL;
    int tmp_int = 0;
    int i = 0;

    utarray_new(tmp_array, &ut_str_icd);
    utarray_new(pCrawl, &node_icd);

    if(root.client_id != NULL){
        utarray_concat(tmp_array, root.client_id);
    }

    if(key[0] == '/'){
        HASH_FIND_STR(root.children, "/", tmpCrawl);
        tmp_int = i = 1;
    }else{
        for(i = 0; key[i] != '\0' && key[i] != '/'; i++);
        tmp_str = (char *) malloc(sizeof(char) * (i + 1));
        memset(tmp_str, 0, sizeof(char) * i);
        strncpy(tmp_str, &key[0], i);
        tmp_str[i] = '\0';
        HASH_FIND_STR(root.children, tmp_str, tmpCrawl);
    }

    if(tmpCrawl != NULL)
        utarray_push_back(pCrawl, tmpCrawl);

    if(root.plus_children != NULL){
        utarray_push_back(pCrawl, root.plus_children);
    }

    if(utarray_front(pCrawl) == NULL){
        return NULL;
    }else{
        for(; key[i] != '\0'; i++){
            if(key[i] == '/'){
                if(key[i + 1] == '/' || i == 0 || key[i + 1] == '\0'){
                    if((test_array = array_hand(pCrawl, "/")) != NULL)
                        utarray_concat(tmp_array, test_array);
                }

                tmp_int = i + 1;
            }else{
                for(i; key[i + 1] != '\0' && key[i + 1] != '/'; i++);
                tmp_str = (char *) malloc(sizeof(char) * (i - tmp_int + 1));
                memset(tmp_str, 0, sizeof(char) * (i - tmp_int + 1));
                strncpy(tmp_str, &key[tmp_int], i - tmp_int + 1);

                if((test_array = array_hand(pCrawl, tmp_str)) != NULL)
                    utarray_concat(tmp_array, test_array);
            }
        }
    }

    while((p = (struct TrieNode *) utarray_next(pCrawl, p))){
        if(p->client_id != NULL)
            utarray_concat(tmp_array, p->client_id);
    }

    deduplication(tmp_array);
    return tmp_array;
}

void delete_client_id(UT_array * array, char * client_id){
    char **first, **find;
    long int pos = 0;

    if((find = utarray_find(array, &client_id, strsort)) != NULL){
        first = utarray_front(array);
        pos = find - first;

        utarray_erase(array, pos, 1);
    }
}

void delete_node(struct TrieNode * node, char * key, char * client_id){
    struct TrieNode * out_node = NULL;
    int key_len = strlen(key);
    int tmp_int = 0;
    char ** p = NULL;
    char * tmp_str;
    char * dest_str;
    int i = 0;

    if(key_len > 0)
        printf("key:%s\n", key);
    else{
        delete_client_id(node->client_id, client_id);
    }

    if(key[0] == '/'){
        if(key[1] == '/' || key[1] == '\0'){
            HASH_FIND_STR(node->children, "/", out_node);
            if(out_node == NULL)
                return;
            else{
                tmp_str = (char *) malloc(sizeof(char) * (key_len + 1));
                memmove(tmp_str, &key[1], key_len - 1);
                delete_node(out_node, tmp_str, client_id);
            }
        }else{
            tmp_str = (char *) malloc(sizeof(char) * (key_len + 1));
            memmove(tmp_str, &key[1], key_len - 1);
            delete_node(node, tmp_str, client_id);
        }
    }else{
        for(i = 0; key[i + 1] != '\0' && key[i + 1] != '/'; i++);
        tmp_str = (char *) malloc(sizeof(char) * (i - tmp_int + 1));
        memset(tmp_str, 0, sizeof(char) * (i - tmp_int + 1));
        strncpy(tmp_str, &key[tmp_int], i - tmp_int + 1);

        if(!strcmp(tmp_str, "#")){
            delete_client_id(node->PoundNode.client_id, client_id);
            return;
        }else if(!strcmp(tmp_str, "+")){
            HASH_FIND_STR(node->plus_children, "+", out_node);
            if(out_node == NULL)
                return;
        }else{
            HASH_FIND_STR(node->children, tmp_str, out_node);
            if(out_node == NULL)
                return;
        }

        tmp_str = (char *) malloc(sizeof(char) * (key_len + 1));
        memmove(tmp_str, &key[1], key_len - 1);
        delete_node(out_node, tmp_str, client_id);
    }

    if(node != NULL){
        printf("fdsfslh\n");
        int flag = 0;
        if(node->children != NULL){
            flag = 1;
        }

        if(node->plus_children != NULL){
            flag = 1;
        }

        if(node->client_id != NULL){
            if(utarray_front(node->client_id) != NULL)
                flag = 1;
        }

        if(node->PoundNode.client_id != NULL){
            if(utarray_front(node->PoundNode.client_id) != NULL)
                flag = 1;
        }

        if(flag == 0){
            printf("return out : %s\n", node->key);
            printf("return : %s\n", node->key);

            if(!strcmp(node->key, "+")){
                HASH_DEL(node->plus_children, node);
            }else{
                HASH_DEL(node->children, node);
            }
            free(node);
            free(out_node);
        }
    }
}

void delete_topic(char * key, char * client_id){
    struct TrieNode * node = NULL;

    char * tmp_str = NULL;
    char **first, **find;
    int key_len = strlen(key);
    int i = 0;
    long int pos = 0;

    if(key[0] == '#'){
        if((find = utarray_find(root.client_id, &client_id, strsort)) != NULL){
            first = utarray_front(root.client_id);
            pos = find - first;

            utarray_erase(root.client_id, pos, 1);
        }

        return;
    }else if(key[0] == '+'){
        HASH_FIND_STR(root.plus_children, "+", node);

        i = 1;
    }else if(key[0] == '/'){
        HASH_FIND_STR(root.children, "/", node);
        
        i = 1;
    }else{
        for(i = 0; key[i] != '\0' && key[i] != '/'; i++);
        
        tmp_str = (char *) malloc(sizeof(char) * (i));
        memset(tmp_str, 0, sizeof(char) * (i));
        strncpy(tmp_str, &key[0], i);
        tmp_str[i] = '\0';
        
        HASH_FIND_STR(root.children, tmp_str, node);
    }
   
    if(node != NULL){
        tmp_str = (char *) malloc(sizeof(char) * (key_len + 1));
        memmove(tmp_str, &key[i], key_len - i);
        delete_node(node, tmp_str, client_id);
    }
}

void delete_all(struct TrieNode * node){
    struct TrieNode *current_user, *tmp;

    if(node == NULL){
        return;
    }

    HASH_ITER(hh, node, current_user, tmp){
        if(current_user->client_id != NULL){
            utarray_clear(current_user->client_id);
        }

        if(current_user->PoundNode.client_id != NULL){
            utarray_clear(current_user->client_id);
        }

        if(current_user->plus_children != NULL){
            delete_all(current_user->plus_children);
        }

        delete_all(current_user->children);
    }

    HASH_CLEAR(hh, node);
}

void printf_all(struct TrieNode * s_root){
    struct TrieNode *current_user, *tmp;
    char **p1 = NULL;
    char **p2 = NULL;

    if(s_root == NULL){
        return;
    }

    HASH_ITER(hh, s_root, current_user, tmp){
        if(current_user->client_id != NULL){
            printf("%s ", current_user->key);
            while((p1=(char**)utarray_next(current_user->client_id,p1))){
                printf("%s ", *p1);
            }

            printf("\n");
        }else{
            printf("%s ", current_user->key);
        }

        if(current_user->PoundNode.client_id != NULL){
            printf("---------------pound------------\n");
            printf("%s ", current_user->key);
            while((p2=(char**)utarray_next(current_user->PoundNode.client_id, p2))){
                printf("%s ", *p2);
            }
            printf("\n");
        }

        if(current_user->plus_children != NULL){
            printf_all(current_user->plus_children);
        }

        printf_all(current_user->children);
    }
}
