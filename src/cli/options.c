#include <stdio.h>     /* for printf */
#include <stdlib.h>    /* for exit */
#include <getopt.h>
#include <errno.h>

#include <string.h>

#include "cli/options.h"


void print_help(const struct cli_option *copts, int32_t optnum)
{
    printf("help: \n");
    for (int i = 0; i < optnum; i++) {
       printf("    -%c, --%s\t%s\n", copts[i].short_name, copts[i].long_name, copts[i].help);
    }
}


struct cli_option *find_option(struct cli_option *copts, int32_t optnum, int c)
{
    for (int i = 0; i < optnum; i++) {
       if (copts[i].short_name == c) {
            return &copts[i];
       }
    }
    return NULL;
}

int parse_options(int32_t argc, char *argv[], const struct cli_option *copts, int32_t optnum)
{
    optind = 1;
    struct option *longopts = (struct option *)calloc(sizeof(struct option), optnum + 2);
    char *optstring = (char *)calloc(sizeof(char), (optnum + 1) * 3 + 1);
    int32_t optstring_idx = 0;

    // longopts[0].name = "help";
    // longopts[0].val = 'h';
    // longopts[0].has_arg = no_argument;
    // longopts[0].flag = NULL;
    // optstring[optstring_idx++] = 'h';
    

    for (int i = 0; i < optnum; i++) {
        // longopts[i].name = copts[i].long_name;
        // longopts[i].val = copts[i].short_name;
        // longopts[i].has_arg = copts[i].flag;
        // longopts[i].flag = NULL;

        switch (copts[i].flag) {
            case no_argument: {
                optstring[optstring_idx++] = copts[i].short_name;
                break;
            }
            case required_argument: {
                optstring[optstring_idx++] = copts[i].short_name;;
                optstring[optstring_idx++] = ':';
                break;
            }
            default: {
                optstring[optstring_idx++] = copts[i].short_name;;
                optstring[optstring_idx++] = ':';
                optstring[optstring_idx++] = ':';
                break;
            }
        }
    }

    

    int option_index = 0;
    int c;
    int ret = 0;
    while ((c = getopt(argc, argv, optstring)) != -1) {
        // if (c == 'h') {
        //     print_help(copts, optnum);
        //     ret = -EINVAL;
        //     break;
        // }
        printf("c = %c   optarg = %s\n", c, optarg);
        struct cli_option *opt = find_option(copts, optnum, c);
        if (opt == NULL) {
            continue;
        }
        void *value = opt->value;
        switch (opt->type) {
            case OPTION_INTEGER: {
                *(uint64_t *)value = strtol(optarg, NULL, 10);
                break;
            }
            case OPTION_DOUBLE: {
                *(double *)value = strtod(optarg, NULL);
                break;
            }
            case OPTION_STRING32: {
                (void)strncpy(value, optarg, 31);
                break;
            }
            case OPTION_STRING64: {
                (void)strncpy(value, optarg, 63);
                break;
            }
            case OPTION_CALLBACK: {
                opt->callback(value, optarg);
                break;
            }
        }
    }
    
    return ret;
}