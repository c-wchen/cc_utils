#include <pthread.h>
#include "fault_inject.h"


typedef struct node {
    struct node *prev;
    struct node *next;
} node_t;

typedef struct {
    fault_inject_item_t item;
    fault_inject_phdl phdl;
    node_t node;
} fault_inject_inner_item_t;


#define FAULT_INJECT_HTBL_SIZE 256
static node_t g_fault_inject_htbl[FAULT_INJECT_HTBL_SIZE] = {0};


fault_inject_item_t *fault_inject_htbl_find(char *name) {
    return NULL;
}

fault_inject_item_t *fault_inject_htbl_create_insert(char *name) {
    fault_inject_inner_item_t *fault = (fault_inject_inner_item_t *)malloc(sizeof(fault_inject_inner_item_t));
    fault->item.sleep_time = 0;
    fault->item.enable_times = INT_MAX;
    fault->item.after_times = 0;
    fault->item.enable = false;
    fault->item.inject_err = 0;
    fault->phdl = NULL;

    
    return;
}

int32_t fault_inject_register(const char *name)
{
    
}
int32_t fault_inject_register_private_handle(const char*name, fault_inject_phdl handle)
{

}

int32_t fault_inject_unregister(const char *name)
{
    
}

fault_inject_item_t *fault_inject_get(char *name, uint64_t private)
{
    fault_inject_inner_item_t *fault = NULL;

    if (fault == NULL) {
        return NULL;
    }
    fault_inject_item_t *item = &fault->item;


    if (item->enable == true) {
        int32_t times = ++item->hit_times;
        if (times < item->after_times ||
            times - item->after_times > item->enable_times ||
            (NULL != fault->phdl && !fault->phdl(item->private, private))) {
            return NULL;
        } else {
            return item;
        }
    }
    return NULL;
}