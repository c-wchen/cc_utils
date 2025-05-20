
#ifndef OPTION_H
#define OPTION_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum option_flag {
    OPTION_FLAG_NO_ARG = 0,
    OPTION_FLAG_REQ_ARG,
    OPTION_FLAG_OPT_ARG,
};


enum option_type {
    OPTION_INTEGER,
    OPTION_DOUBLE,
    OPTION_STRING32,
    OPTION_STRING64,
    OPTION_CALLBACK,
};

typedef int opt_cb(void *value, const char *optarg);

struct cli_option {
    enum option_type type;
    enum option_flag flag;
	int short_name;
	const char *long_name;
	void *value;
	const char *argh;
	const char *help;
	opt_cb *callback;
	intptr_t defval;
};

int parse_options(int32_t argc, char *argv[], const struct cli_option *options, int32_t optnum);

#define OPT_INT(s, l, v, h) {               \
    .type = OPTION_INTEGER,                 \
    .flag = OPTION_FLAG_REQ_ARG,            \
    .short_name = (s),                      \
    .long_name = (l),                       \
    .value = (void *)(v),                   \
    .argh = (h),                            \
    .help = (h),                            \
    .callback = NULL,                       \
}

#define OPT_STRING32(s, l, v, h) {          \
    .type = OPTION_STRING32,                \
    .flag = OPTION_FLAG_REQ_ARG,            \
    .short_name = (s),                      \
    .long_name = (l),                       \
    .value = (void *)(v),                   \
    .argh = (h),                            \
    .help = (h),                            \
    .callback = NULL,                       \
}


#define OPT_STRING64(s, l, v, h) {          \
    .type = OPTION_STRING64,                \
    .flag = OPTION_FLAG_REQ_ARG,            \
    .short_name = (s),                      \
    .long_name = (l),                       \
    .value = (void *)(v),                   \
    .argh = (h),                            \
    .help = (h),                            \
    .callback = NULL,                       \
}


#define OPT_CALLBACK(s, l, v, h, cb) {      \
    .type = OPTION_CALLBACK,                \
    .flag = OPTION_FLAG_REQ_ARG,            \
    .short_name = (s),                      \
    .long_name = (l),                       \
    .value = (void *)(v),                   \
    .argh = (h),                            \
    .help = (h),                            \
    .callback = (cb),                       \
}


#define OPT_DOUBLE(s, l, v, h) {            \
    .type = OPTION_DOUBLE,                  \
    .flag = OPTION_FLAG_REQ_ARG,            \
    .short_name = (s),                      \
    .long_name = (l),                       \
    .value = (void *)(v),                   \
    .argh = (h),                            \
    .help = (h),                            \
    .callback = NULL,                       \
}


#ifdef __cplusplus /* OPTION_H */
}
#endif
#endif