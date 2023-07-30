#include "kernel/types.h"
#include "user/user.h"

void send_nums_and_wait(int write_end)
{
    int prime_candidate;
    for (prime_candidate = 2; prime_candidate <= 35; prime_candidate++)
    {
        if (write(write_end, &prime_candidate, sizeof(prime_candidate)) == -1)
        {
            fprintf(2, "Error: writing %d\n", prime_candidate);
            exit(1);
        }
    }
    close(write_end);
    wait(0);
}

void print_prime_and_send_nums(int parent_read_end)
{
    int child_pid;
    do
    {
        int prime;
        int read_result = read(parent_read_end, &prime, sizeof(prime));
        if (read_result == 0)
        {
            break;
        }
        if (read_result == -1)
        {
            fprintf(2, "Error: read fd[0]\n");
            exit(1);
        }
        printf("prime %d\n", prime);

        int cur_fd[2];
        if (pipe(cur_fd) == -1)
        {
            fprintf(2, "Error: pipe(cur_fd)\n");
            exit(1);
        }

        child_pid = fork();
        if (child_pid == -1)
        {
            fprintf(2, "Error: fork()\n");
            exit(1);
        }

        if (child_pid) // current
        {
            close(cur_fd[0]);
            int prime_candidate, read_result;
            while ((read_result = read(parent_read_end, &prime_candidate, sizeof(prime_candidate))))
            {
                if (read_result == -1)
                {
                    fprintf(2, "Error: read fd[0]\n");
                    exit(1);
                }
                if (prime_candidate % prime == 0)
                {
                    continue;
                }
                write(cur_fd[1], &prime_candidate, sizeof(prime_candidate));
            }
            close(parent_read_end);
            close(cur_fd[1]);
            wait(0);
        }
        else // child
        {
            close(parent_read_end);
            close(cur_fd[1]);
            parent_read_end = cur_fd[0];
        }
    } while (child_pid == 0);
}
int main(void)
{
    int fd[2];
    if (pipe(fd) == -1)
    {
        fprintf(2, "Error: pipe(fd)\n");
        exit(1);
    }
    int child_pid = fork();
    if (child_pid == -1)
    {
        fprintf(2, "Error: fork()\n");
        exit(1);
    }
    if (child_pid) // parent
    {
        close(fd[0]);
        send_nums_and_wait(fd[1]);
    }
    else // child
    {
        close(fd[1]);
        print_prime_and_send_nums(fd[0]);
    }
    return 0;
}