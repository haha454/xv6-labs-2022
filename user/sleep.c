#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int secs;

  if(argc <= 1) {
    fprintf(2, "sleep: lack of arguments\n");
    exit(1);
  }

  secs = atoi(argv[1]);
  fprintf(1, "sleep: going to sleep for %d seconds\n", secs);
  sleep(secs);
  exit(0);
}
