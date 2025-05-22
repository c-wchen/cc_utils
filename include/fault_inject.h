
#ifndef FAULT_H
#define FAULT_H

#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FAULT_INJECT_NAME_SIZE 64

typedef bool (*priv_cb)(uint64_t arg, uint64_t priv);

/*
 * set fault_inejct name xxx switch xxx after_times xxx enable_times xxx sleep_time xxx inject_err xxx private xxx
 * show fault_inject
 */

typedef struct {
    char name[FAULT_INJECT_NAME_SIZE];
    bool enable;
    int32_t sleep_time;
    int32_t after_times;
    int32_t enable_times;
    int32_t inject_err;
    int32_t hit_times;
    uint64_t priv;
} fault_inject_t;

int32_t fault_inject_register(const char *name);
int32_t fault_inject_register_priv(const char *name,  priv_cb cb);
int32_t fault_inject_unregister(const char *name);

void fault_inject_enable(const char *name);
void fault_inject_disable(const char *name);
void fault_inject_reinit(const char *name);
void fault_inject_init(const char *name, fault_inject_t new_fault);
fault_inject_t *fault_inject_find(const char *name);
fault_inject_t *fault_inject_cond_get(const char *name, uint64_t priv_arg);

/*
 * FAULT_INJECT_BEGIN(xxx, FAULT_STATEMENT_MOD_ERR(ret), 0);
 * function call;
 * FAULT_INJECT_END(xxx)
 */
#define FAULT_STATEMENT_MOD_ERR(ret) {(ret) = __fault_inject_item->inject_err;}
#define FAULT_STATEMENT_RET_ERR(ret) {return __fault_inject_item->inject_err;}
#define FAULT_STATEMENT_ASSERT {assert(!("fault inject assert!"));}

#define FAULT_INJECT_BEGIN(name, statement, priv_arg)                            \
    fault_inject_t *__fault_inject_item = fault_inject_cond_get(name, priv_arg); \
    if (NULL != __fault_inject_item) {                                           \
        statement;                                                               \
    } else {

#define FAULT_INJECT_END(name)   }

#ifdef __cplusplus
}
#endif
#endif