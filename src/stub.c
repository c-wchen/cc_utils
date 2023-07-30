#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/mman.h>
#include "stub.h"
#define MPROTECT_RWX  (PROT_READ | PROT_WRITE | PROT_EXEC)
#define MPROTECT_RX   (PROT_READ | PROT_EXEC)
#define PAGE_SIZE (1 << 12)

static inline uintptr_t
alignpage(uintptr_t v) {
  return (v & ~((uintptr_t)(PAGE_SIZE - 1u)));
}

static inline int
mprotect_write(void *fn) {
  int ret;
  void *ap = (void *)alignpage((uintptr_t)fn);
  ret = mprotect(ap, PAGE_SIZE, MPROTECT_RWX);
  return ret;
}

static inline int
mprotect_recovery(void *fn) {
  int ret;
  void *ap = (void *)alignpage((uintptr_t)fn);
  ret = mprotect(ap, PAGE_SIZE, MPROTECT_RX);
  return ret;
}

stub_s *
stub_init(size_t stub_len) {
  stub_s *m = calloc(1, STUB_LEN + stub_len);
  m->len = stub_len;
  m->stub = (char *) (m + sizeof(stub_s));
  return m;
}

int stub_set(stub_s *m, void *fn, void *stub_fn) {
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
  memcpy(m->stub, fn, jmpq_op_len);
  *((uint8_t *)fn) = STUB_JMPQ;
  opaddr = fn + STUB_JMPQ_LEN;
  *((uint32_t *)opaddr) = stub_fn - fn - STUB_LEN;
  ret = mprotect_recovery(fn);

  if (ret) {
    printf("%s mprotect_recovery error!\n", __func__);
    return ret;
  }
  return ret;
}

int
stub_reset(stub_s *m) {
  int ret;
  void *fn;
  fn = m->fn;
  ret = mprotect_write(fn);
  if (ret) {
    printf("%s mprotect_write error!\n", __func__);
    return ret;
  }
  memcpy(fn, m->stub, m->len);
  ret = mprotect_recovery(fn);
  if (ret) {
    printf("%s mprotect_recovery error!\n", __func__);
    return ret;
  }
  return ret;
}

