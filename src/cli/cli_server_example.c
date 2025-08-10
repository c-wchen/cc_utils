#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#include "cli/cli.h"
#include "cli/options.h"

#include "fault_inject.h"

typedef struct {
    char name[32];
    int32_t age;
    int32_t sex;
    double score;
    bool pass;
} student;

static void cmd_student(void *cdp, int32_t argc, char **argv)
{
    printf("exec cmd student....\n");
    student s = {0};
    struct cli_option copts[] = {
        OPT_STRING32('n', "name", &s.name, "student name"),
        OPT_INT('a', "age", &s.age, "student age"),
        OPT_INT('s', "sex", &s.sex, "student sex"),
        OPT_DOUBLE('S', "score", &s.score, "student score"),
        OPT_BOOL('p', "pass", &s.pass, "student score")
    };
    int ret = parse_options(cdp, argc, argv, copts, 5);

    if (ret != 0) {
        return;
    }

    CMD_PRINT(cdp, "==> name: %s\n", s.name);
    CMD_PRINT(cdp, "==> age: %d\n", s.age);
    CMD_PRINT(cdp, "==> sex: %d\n", s.sex);
    CMD_PRINT(cdp, "==> score: %lf\n", s.score);
    CMD_PRINT(cdp, "==> pass: %s\n", s.pass ? "true" : "false");

    return;
}

static void cmd_fault(void *cdp, int32_t argc, char **argv)
{
    printf("exec cmd fault....\n");
    fault_inject_t fault = {0};
    struct cli_option copts[] = {
        OPT_STRING64('n', "name", &fault.name, "fault name"),
        OPT_BOOL('e', "enable", &fault.enable, "fault switch"),
        OPT_INT('s', "sleep_time", &fault.sleep_time, "faullt sleep time"),
        OPT_INT('a', "after_times", &fault.after_times, "fault after times"),
        OPT_INT('E', "enable_times", &fault.enable_times, "fault enable times"),
        OPT_INT('r', "inject_err", &fault.inject_err, "fault inject error"),
        OPT_INT('H', "hit_times", &fault.hit_times, "fault hit times"),
        OPT_LONG('p', "private", &fault.priv, "fault private data")
    };
    int ret = parse_options(cdp, argc, argv, copts, sizeof(copts) / sizeof(copts[0]));

    if (ret != 0) {
        return;
    }

    CMD_PRINT(cdp, "==> name: %s\n", fault.name);
    CMD_PRINT(cdp, "==> enable: %d\n", fault.enable);
    CMD_PRINT(cdp, "==> sleep_time: %d\n", fault.sleep_time);
    CMD_PRINT(cdp, "==> after_time: %d\n", fault.after_times);
    CMD_PRINT(cdp, "==> enable_times: %d\n", fault.enable_times);
    CMD_PRINT(cdp, "==> inject_err: %d\n", fault.inject_err);
    CMD_PRINT(cdp, "==> hit_times: %d\n", fault.hit_times);
    CMD_PRINT(cdp, "==> private: %lx\n", fault.priv);

    return;
}

int main()
{
    cli_create("wchen");

    (void)cli_register("stu", "query student", cmd_student);
    (void)cli_register("fault", "fault inject", cmd_fault);

    while (1) {
        sleep(20);
    }

    printf("start destroy...\n");

    cli_destroy();
    return 0;
}