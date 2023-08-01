#include <stdbool.h>

#include "kernel/types.h"
#include "user/user.h"

#define MAX_ARG_LEN 512

void do_command(int argc, char *argv[])
{
    if (fork()) // parent
    {
        wait(0);
    }
    else // child
    {
        exec(argv[0], argv);
    }
}

int main(int argc, char *argv[])
{
    memmove(argv, argv + 1, (argc - 1) * sizeof(char *));

    char buf[MAX_ARG_LEN];
    while (true)
    {
        memset(buf, 0, MAX_ARG_LEN);
        gets(buf, MAX_ARG_LEN);
        if (!strlen(buf)) {
            break;
        }
        buf[strlen(buf) - 1] = '\0';
        argv[argc - 1] = buf;
        do_command(argc, argv);
    }

    return 0;
}