#include <stdio.h>

#define FAULT_INJECT_NAME_SIZE 64

typedef bool (*fault_inject_phdl)(uint64_t outside, uint64_t inside);

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
    uint64_t private;
} fault_inject_item_t;

int32_t fault_inject_register(const char *name);
int32_t fault_inject_register_private_handle(const char *name, fault_inject_phdl handle);

int32_t fault_inject_unregister(const char *name);

fault_inject_item_t *fault_inject_get(char *name, uint64_t private);

/*
 * FAULT_INJECT_BEGIN(xxx, FAULT_STATEMENT_MOD_ERR(ret), 0);
 * function call;
 * FAULT_INJECT_END(xxx)
 */
#define FAULT_STATEMENT_MOD_ERR(ret) {(ret) = __fault_inject_item->inject_err;}
#define FAULT_STATEMENT_RET_ERR(ret) {return __fault_inject_item->inject_err;}

#define FAULT_INJECT_BEGIN(name, statement, private)                             \
    fault_inject_item *__fault_inject_item = fault_inject_get(#name, private)    \
    if (NULL != __fault_inject_item) {                                           \
        statement;                                                               \
    } else {

#define FAULT_INJECT_END(name)   }