#include "kernel/types.h"
#include "user/user.h"

int main(void)
{
    int p[2];
    if (pipe(p) == -1)
    {
        fprintf(2, "Error: pipe(p) error.\n");
    }

    if (fork()) // parent
    {
        char read_buffer;
        write(p[1], "X", 1);
        close(p[1]);
        read(p[0], &read_buffer, 1);
        fprintf(1, "%d: received pong\n", getpid(), read_buffer);
        close(p[0]);
    }
    else // child
    {
        char read_buffer;
        read(p[0], &read_buffer, 1);
        close(p[0]);
        fprintf(1, "%d: received ping\n", getpid(), read_buffer);
        write(p[1], "Y", 1);
        close(p[1]);
    }
    exit(0);
}