#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "common.h"

int main(int argc, char *argv[]) {
  int fd;
  char name[] = "fast_sender";

  printf("starting %s\n", name);

  if (argc < 2) {
    printf("%s must be called with the fd of the pipe to use.\n", name);
    fflush(NULL);
    return 1;
  }

  fd = atoi(argv[1]);
  
  if (fd < 0) {
    sleep(1);
    printf("%s: fake mode\n", name);
    fflush(NULL);
    return 0;
  }

  if (write_pipe(name, FAST, fd)) {
    fflush(NULL);
    return 1;
  }

  fflush(NULL);
  return 0;
}
