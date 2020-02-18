// init: The initial user-level program

#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

char *argv[] = { "sh", 0 };

void 
bootmsg(){
  printf(1, "--------------------------------- \n");
  printf(1, "--------- Group Members --------- \n");
  printf(1, "- 1. Mohammad Rabiei: 810195395 - \n");
  printf(1, "- 2. Sharare Norouzi: 810097015 - \n");
  printf(1, "- 3. Ali Ranjbar    : 810097029 - \n");
  printf(1, "--------------------------------- \n");
}


int
main(void)
{
  int pid, wpid;

  if(open("console", O_RDWR) < 0){
    mknod("console", 1, 1);
    open("console", O_RDWR);
  }
  dup(0);  // stdout
  dup(0);  // stderr
  bootmsg();


  for(;;){
    printf(1, "init: starting sh\n");

    pid = fork();
    if(pid < 0){
      printf(1, "init: fork failed\n");
      exit();
    }

    if(pid == 0){
      exec("sh", argv);
      printf(1, "init: exec sh failed\n");
      exit();
    }
    while((wpid=wait()) >= 0 && wpid != pid)
      printf(1, "zombie!\n");
  }
}
