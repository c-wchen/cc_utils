//
// Created by 忘尘 on 2022/10/28.
//
#include <stdint.h>

typedef void (*TpCallback)(void *param)
typedef struct {
    TpCallback tpCallback;
} TracePoint;

#define MAX_TP_DEFAULT_NUM 1000
#define MAX_MOD_DEFAULT_NUM 100
TracePoint g_tracePoint[MAX_MOD_DEFAULT_NUM][MAX_TP_DEFAULT_NUM] = {{0}};

void RegisterTp(int32_t module, int32_t index, TpCallback *tp) {
    if (module < 0  || module >= MAX_MOD_DEFAULT_NUM || index < 0 || index >= MAX_TP_DEFAULT_NUM) {
        printf("sys tp register faild.\n");
    }
    g_tracePoint[module][index].tpCallback = tp;
}

#define TP_START(mod, index, param) do { \
    (*g_tracePoint[module][index].tpCallback)(param); \

#define TP_END \
    } while(0);
