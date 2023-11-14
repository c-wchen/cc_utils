#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/mman.h>
#include "mock.h"
#define MPROTECT_RWX (PROT_READ | PROT_WRITE | PROT_EXEC)
#define MPROTECT_RX (PROT_READ | PROT_EXEC)
#define PAGE_SIZE (1 << 12)

static inline uintptr_t
alignpage(uintptr_t v)
{
    return (v & ~((uintptr_t)(PAGE_SIZE - 1u)));
}

static inline int
mprotect_write(void *fn)
{
    int ret;
    void *ap = (void *)alignpage((uintptr_t)fn);
    ret = mprotect(ap, PAGE_SIZE, MPROTECT_RWX);
    return ret;
}

static inline int
mprotect_recovery(void *fn)
{
    int ret;
    void *ap = (void *)alignpage((uintptr_t)fn);
    ret = mprotect(ap, PAGE_SIZE, MPROTECT_RX);
    return ret;
}

mock_s *
mock_init(size_t mock_len)
{
    mock_s *m = calloc(1, sizeof(mock_s) + mock_len);
    m->len = mock_len;
    m->mock = (char *)((char *)m + sizeof(mock_s));
    return m;
}

int mock_set(mock_s *m, void *fn, void *mock_fn)
{
    int ret = 0;
    void *opaddr;
    size_t jmpq_op_len;

    ret = mprotect_write(fn);
    if (ret) {
        printf("%s mprotect_write error!\n", __func__);
        return ret;
    }
    m->fn = fn;
    jmpq_op_len = m->len;
    memcpy(m->mock, fn, jmpq_op_len);
    *((uint8_t *)fn) = MOCK_JMPQ;
    opaddr = fn + MOCK_JMPQ_LEN;
    *((uint32_t *)opaddr) = mock_fn - fn - MOCK_LEN;
    ret = mprotect_recovery(fn);

    if (ret) {
        printf("%s mprotect_recovery error!\n", __func__);
        return ret;
    }
    return ret;
}

int mock_reset(mock_s *m)
{
    int ret;
    void *fn;
    fn = m->fn;
    ret = mprotect_write(fn);
    if (ret) {
        printf("%s mprotect_write error!\n", __func__);
        return ret;
    }
    memcpy(fn, m->mock, m->len);
    ret = mprotect_recovery(fn);
    if (ret) {
        printf("%s mprotect_recovery error!\n", __func__);
        return ret;
    }
    return ret;
}
