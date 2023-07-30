#include "stub.h"
#include "gtest/gtest.h"
#include <stdint.h>

static void
test_stub_fn(void) {
  printf("%s\n", __func__);
}

static void
test_fn(void) {
  printf("%s\n", __func__);
}

void 
test_void_fn() {

}

TEST(TEST_STUB, test_custom_stub)  {
  stub_s *m;
  m = stub_init(STUB_LEN);

  int ret = stub_set(m, (void *)test_fn, (void *)test_stub_fn);
  EXPECT_EQ(ret, 0);
  test_fn();
  stub_reset(m);
  test_fn();
  stub_finalize(m);
}
