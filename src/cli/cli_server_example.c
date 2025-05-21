#include <unistd.h>
#include <stdio.h>

#include "cli/cli.h"
#include "cli/options.h"

typedef struct {
    char name[32];
    int32_t age;
    int32_t sex;
    double score;
} student;

static void cmd_student(void *cdp, int32_t argc, char** argv)
{
    printf("exec cmd student....\n");
    student s;
    struct cli_option copts[] = {
        OPT_STRING32('n', "name", &s.name, "student name"),
        OPT_INT('a', "age", &s.age, "student age"),
        OPT_INT('s', "sex", &s.sex, "student sex"),
        OPT_DOUBLE('S', "score", &s.score, "student score")
    };
    int ret = parse_options(argc, argv, copts, 4);

    if (ret != 0) {
        return;
    }

    CMD_PRINT(cdp, "==> name: %s\n", s.name);
    CMD_PRINT(cdp, "==> age: %d\n", s.age);
    CMD_PRINT(cdp, "==> sex: %d\n", s.sex);
    CMD_PRINT(cdp, "==> score: %lf\n", s.score);

    return;
}

int main()
{
    cli_create("wchen");

    (void)cli_register("stu", "query student", cmd_student);

    printf("cmd registedr success...\n");
    
    while (1) {
        sleep(20);
    }
    
    printf("start destroy...\n");

    cli_destroy();
    return 0;
}