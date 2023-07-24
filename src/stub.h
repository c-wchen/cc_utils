#ifndef _STUB_H_
#define _STUB_H_

#define STUB_JMPQ 0XE9
#define STUB_JMPQ_LEN (1)
#define STUB_JMPQ_OPLEN (4)
#define STUB_LEN (STUB_JMPQ_LEN + STUB_JMPQ_OPLEN)

typedef struct {
  void *fn;
  char *stub;
  size_t len;
}

stub_s * stub_init(size_t stub_len);
int stub_reset(stub_s *m);
int stub_set(stub_s *m, void *fn, void *stub_fn);
int stub_reset(stub_s *m);

static inline void
stub_finalize(stub_s *m) {
  free(m);
}

#endif /* _STUB_H_ */
