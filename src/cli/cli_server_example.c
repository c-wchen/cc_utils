#include <unistd.h>
#include <stdio.h>

#include "cli/cli.h"

int main()
{
    cli_create("wchen");
    
    while (1) {
        sleep(20);
    }
    
    printf("start destroy...\n");

    cli_destroy();
    return 0;
}