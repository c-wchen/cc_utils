#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>
#include <pthread.h>

#include <list.h>
#include "fault_inject.h"

typedef struct {
    fault_inject_item_t item;
    dl_node_t node;
} fault_inject_inner_item_t;

#define FAULT_INJECT_HTBL_SIZE 256
dl_head_t faults = {0};

fault_inject_item_t *fault_inject_find(char *name)
{
    fault_inject_inner_item_t *fault;
    DL_FOREACH_ENTRY(&faults, fault, node) {
        if (strncmp(fault->item.name, name, FAULT_INJECT_NAME_SIZE - 1) == 0) {
            return &fault->item;
        }
    }
    return NULL;
}

void fault_inject_del(const char *name)
{
    fault_inject_inner_item_t *fault, *next;
    DL_FOREACH_ENTRY_SAFE(&faults, fault, next, node) {
        if (strncmp(fault->item.name, name, FAULT_INJECT_NAME_SIZE) == 0) {
            dl_del(&fault->node);
            free(fault);
            return;
        }
    }
    return;
}

fault_inject_item_t *fault_inject_insert(const char *name)
{
    fault_inject_inner_item_t *fault = (fault_inject_inner_item_t *)malloc(sizeof(fault_inject_inner_item_t));
    fault->item.sleep_time = 0;
    fault->item.enable_times = INT_MAX;
    fault->item.after_times = 0;
    fault->item.enable = false;
    fault->item.inject_err = 0;
    strncpy(fault->item.name, name, FAULT_INJECT_NAME_SIZE - 1);

    dl_add_head(&faults, &fault->node);

    return &fault->item;
}

int32_t fault_inject_register(const char *name)
{
    fault_inject_insert(name);
    return 0;
}


int32_t fault_inject_register_private_handle(const char *name, fault_inject_phdl handle)
{

}

int32_t fault_inject_unregister(const char *name)
{
    fault_inject_del(name);
}

fault_inject_item_t *fault_inject_get(char *name, uint64_t private)
{
    fault_inject_item_t *fault = fault_inject_find(name);

    if (fault == NULL) {
        return NULL;
    }


    if (fault->enable == true) {
        int32_t times = ++fault->hit_times;
        if (times < fault->after_times ||
            times - fault->after_times > fault->enable_times) {
            return NULL;
        } else {
            return fault;
        }
    }
    return NULL;
}