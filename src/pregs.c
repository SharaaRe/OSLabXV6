#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int
main(int argc, char *argv[])
{
  read_registers();
  if (argc == 2) {
      set_edx(atoi(argv[1]));
  } else {
    set_edx(0x414243);
  }
  read_registers();

  exit();
}
