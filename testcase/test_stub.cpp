#inlcude "stub.h"
#include "gtest/gtest.h"

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

  ret = stub_set(m, test_fn, test_stub_fn);
  if (ret) {
    return -1;
  }
  test_fn();
  stub_reset(m);
  test_fn();
  stub_finalize(m);
  return 0;  
}
