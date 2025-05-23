#include <stdio.h>     /* for printf */
#include <stdlib.h>    /* for exit */
#include <getopt.h>
#include <errno.h>
#include <stdbool.h>

#include <string.h>

#include "cli/cli.h"
#include "cli/options.h"

void print_help(void *cdp, const struct cli_option *copts, int32_t optnum)
{
    CMD_PRINTLN(cdp, "help:");
    for (int i = 0; i < optnum; i++) {
        CMD_PRINTLN(cdp, "    -%c, --%s\t%s", copts[i].short_name, copts[i].long_name, copts[i].help);
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

int parse_options(void *cdp, int32_t argc, char *argv[], const struct cli_option *copts, int32_t optnum)
{
    optind = 1;
    struct option *longopts = (struct option *)calloc(sizeof(struct option), optnum + 2);
    char *optstring = (char *)calloc(sizeof(char), (optnum + 1) * 3 + 1);
    int32_t optstring_idx = 0;

    longopts[0].name = "help";
    longopts[0].val = 'h';
    longopts[0].has_arg = no_argument;
    longopts[0].flag = NULL;
    optstring[optstring_idx++] = 'h';

    for (int i = 0; i < optnum; i++) {
        longopts[i + 1].name = copts[i].long_name;
        longopts[i + 1].val = copts[i].short_name;
        longopts[i + 1].has_arg = copts[i].flag;
        longopts[i + 1].flag = NULL;

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

    char **new_argv = (char **)calloc(sizeof(char **), argc + 1);
    new_argv[0] = "temp";
    for (int i = 0; i < argc; i++) {
        new_argv[i + 1] = argv[i];
    }

    int option_index = 0;
    int c;
    int ret = 0;
    while ((c = getopt_long(argc + 1, new_argv, optstring, longopts, &option_index)) != -1) {
        if (c == 'h') {
            print_help(cdp, copts, optnum);
            ret = -EINVAL;
            break;
        }
        struct cli_option *opt = find_option(copts, optnum, c);
        if (opt == NULL) {
            continue;
        }
        void *value = opt->value;
        switch (opt->type) {
            case OPTION_INT: {
                *(int32_t *)value = strtol(optarg, NULL, 10);
                break;
            }
            case OPTION_LONG: {
                *(int64_t *)value = strtol(optarg, NULL, 10);
                break;
            }
            case OPTION_BOOL: {
                *(bool *)value = true;
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

    free(new_argv);
    free(optstring);
    free(longopts);

    return ret;
}