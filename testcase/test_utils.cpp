#include "gtest/gtest.h"
#include "num_convert.h"
#include "malloc_plus.h"
#include "check_param.h"
#include <limits.h>

TEST(TEST_UTILS, test_str_to_int) {
    EXPECT_NE(str_to_int("1234", 4), INT_MIN);
    EXPECT_EQ(str_to_int("test", 4), INT_MIN);
    EXPECT_EQ(str_to_int("1111111111111111", 16), INT_MIN);
}

typedef struct {
    double val; // float存在精度损失
    char *input;
    int preserve;
} FloatToStrStu;
TEST(TEST_UTILS, test_float_to_str) {
    FloatToStrStu ts[] = {
            {365.40600412823,    "365.406",   3},
            {131.0673001659629,  "131.067",   3},
            {7.6627825411210555, "7.7",       1},
            {845.7148309173031,  "845.715",   3},
            {24.972874441160187, "24.97",     2},
            {3.1749739346876673, "3.2",       1},
            {32.31985242273748,  "32.32",     2},
            {91.1496975540085,   "91.15",     2},
            {2.763879751523659,  "2.8",       1},
            {0.6021928597465853, "0.6",       1},
            {961.6796446580618,  "961.680",   3},
            {8.719778071241969,  "8.72",      2},
            {3973.3154381839877, "3973.3154", 4},
            {3.8832864851559945, "3.9",       1},
            {86.47437267037523,  "86.47",     2},
            {764.0431627372444,  "764.043",   3},
            {8721.0927678181,    "8721.0928", 4},
            {5672.861332671666,  "5672.8613", 4},
            {768.2726247521332,  "768.273",   3},
            {1.2558699345022961, "1.26",      2},
            {9792.255676484427,  "9792.2557", 4},
            {18.873243273750628, "18.87",     2},
            {18.872855153706436, "18.87",     2},
            {738.0025384858047,  "738.003",   3},
            {191.735105632999,   "191.735",   3},
            {4.4713558572459275, "4.5",       1},
            {95.22888028957281,  "95.23",     2},
            {7.010089191368492,  "7.0",       1},
            {2.7101398544323176, "2.7",       1},
            {20.63854912791382,  "20.64",     2},
    };
    int len = sizeof(ts) / sizeof(FloatToStrStu);
    char output[100] = {0};
    for (int i = 0; i < len; ++i) {
        memset(output, 0, 100);
        double_to_str(ts[i].val, ts[i].preserve, output, 100);
        EXPECT_STREQ(output, ts[i].input);
    }
}

TEST(TEST_UTILS, test_malloc_plus) {
    int *a = (int *) MALLOC_PLUS(10 * sizeof(int));
    a[0] = 11;
    for (int i = 0; i < 10; ++i) {
        printf("index %d value %d\n", i, *(a + i));
    }
    FREE_PLUS(a);
}

TEST(TEST_UTILS, test_chec_param) {
    uint64_t a = 0;
    uint64_t b = 90;
    uint64_t c = 0;
    EXPECT_EQ(CHECK_ADDR(PARAM_THREE, a, b, c), true);
}