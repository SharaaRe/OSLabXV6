#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int
main(int argc, char *argv[])
{
  // if(fork() == 0) {
  //   exit();
  // }

  // wait();
  print_syscalls();

  exit();
}
