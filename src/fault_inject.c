#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include "list.h"
#include "rbtree.h"
#include "fault_inject.h"

typedef struct {
    fault_inject_t item;
    priv_cb callback;
    struct rb_node node;
} fault_inject_internal_t;


typedef struct {
    struct rb_root root;
    /* 单线程注册可以实现免锁，调用者约束 */
    /* pthread_rwlock_t rwlock; */
} fault_gloabl_t;

fault_gloabl_t faults = {0};

static int cmp_node(struct rb_node *a, const struct rb_node *b)
{
    fault_inject_internal_t *f1 = rb_entry(a, fault_inject_internal_t, node);
    fault_inject_internal_t *f2 = rb_entry(b, fault_inject_internal_t, node);

    return strncmp(f1->item.name, f2->item.name, FAULT_INJECT_NAME_SIZE);
}

static int cmp_key(const void *key, const struct rb_node *a)
{
    fault_inject_internal_t *f = rb_entry(a, fault_inject_internal_t, node);
    return strncmp(key, f->item.name, FAULT_INJECT_NAME_SIZE);
}

fault_inject_t *fault_inject_find(const char *name)
{
    struct rb_node *node = rb_find_first(&faults.root, name, cmp_key);
    if (node == NULL) {
        return NULL;
    } else {
        return &(rb_entry(node, fault_inject_internal_t, node)->item);
    }
}

static fault_inject_internal_t *fault_inject_del(const char *name)
{
    struct rb_node *find = rb_find_first(&faults.root, name, cmp_key);

    if (find == NULL) {
        return NULL;
    }
    rb_erase(find, &faults.root);
    
    return rb_entry(find, fault_inject_internal_t, node);
}

void fault_inject_reset(fault_inject_t *fault)
{
    fault->sleep_time = 0;
    fault->enable_times = INT_MAX;
    fault->after_times = 0;
    fault->enable = false;
    fault->inject_err = 0;
    fault->hit_times = 0;
    return;
}


void fault_inject_disable(const char *name)
{
    fault_inject_t *fault = fault_inject_find(name);
    if (fault != NULL) {
        fault->enable = false;
    }
    return;
}

void fault_inject_enable(const char *name)
{
    fault_inject_t *fault = fault_inject_find(name);
    if (fault != NULL) {
        fault->enable = true;
    }
    return;
}

void fault_inject_reinit(const char *name)
{
    fault_inject_t *fault = fault_inject_find(name);
    if (fault != NULL) {
        fault_inject_reset(fault);
    }
    return;
}

void fault_inject_init(const char *name, fault_inject_t new_fault)
{
    fault_inject_t *fault = fault_inject_find(name);
    if (fault != NULL) {
        fault->sleep_time = new_fault.sleep_time;
        fault->enable_times = new_fault.enable_times;
        fault->after_times = new_fault.after_times;
        fault->enable = new_fault.enable;
        fault->inject_err = new_fault.inject_err;
        fault->hit_times = new_fault.hit_times;
    }
    return;
}

static fault_inject_t *fault_inject_insert(const char *name)
{
    fault_inject_internal_t *f = (fault_inject_internal_t *)malloc(sizeof(fault_inject_internal_t));
    strncpy(f->item.name, name, FAULT_INJECT_NAME_SIZE - 1);
    f->callback = NULL;
    fault_inject_reset(&f->item);

    rb_add(&faults.root, &f->node, cmp_node);

    return &f->item;
}

int32_t fault_inject_register(const char *name)
{
    if (fault_inject_find(name) == NULL) {
        fault_inject_insert(name);
    }
    return 0;
}


int32_t fault_inject_register_priv(const char *name,  priv_cb cb)
{
    
    if (fault_inject_find(name) == NULL) {
        fault_inject_t *fault = fault_inject_insert(name);
        fault_inject_internal_t *internal = container_of(fault, fault_inject_internal_t, item);
        internal->callback = cb;
    }
}

int32_t fault_inject_unregister(const char *name)
{
    fault_inject_internal_t *internal = fault_inject_del(name);
    if (internal)
        free(internal);
    return 0;
}

fault_inject_t *fault_inject_cond_get(const char *name, uint64_t priv_arg)
{
    fault_inject_t *fault = fault_inject_find(name);

    if (fault == NULL) {
        return NULL;
    }

    if (fault->enable == true) {
        int32_t times = ++fault->hit_times;
        if (times <= fault->after_times ||
            times - fault->after_times > fault->enable_times) {
            return NULL;
        } else {
            fault_inject_internal_t *internal = container_of(fault, fault_inject_internal_t, item);
            if (internal->callback == NULL || internal->callback(priv_arg, fault->priv)) {
                if (fault->sleep_time > 0)
                    sleep(fault->sleep_time);
                return fault;
            }
        }
    }
    return NULL;
}