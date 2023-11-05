#ifndef _MOCK_H_
#define _MOCK_H_

#define MOCK_JMPQ 0XE9
#define MOCK_JMPQ_LEN (1)
#define MOCK_JMPQ_OPLEN (4)
#define MOCK_LEN (MOCK_JMPQ_LEN + MOCK_JMPQ_OPLEN)
#include <stddef.h>
#include <stdlib.h>

typedef struct {
  void *fn;
  char *mock;
  size_t len;
} mock_s;

#ifdef __cplusplus
extern "C" {
#endif

mock_s * mock_init(size_t mock_len);
int mock_reset(mock_s *m);
int mock_set(mock_s *m, void *fn, void *mock_fn);
int mock_reset(mock_s *m);

static inline void
mock_finalize(mock_s *m) {
  free(m);
}

#ifdef __cplusplus
}
#endif

#endif /* _MOCK_H_ */
