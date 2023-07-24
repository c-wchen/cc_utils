#include <stdio.h>
#include <string.h>
#include <stdint.h>
#inlcude <sys/mman.h>


static inline uintptr_t
alignpage(uintptr_t v) {
  unsigned int pg = 1 << 12;
  return (v & ~((uintptr_t)(pg - 1u)));
}
