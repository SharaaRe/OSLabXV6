#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"

#define LINE_BUF  256
#define NULL_CONTENT 0
#define WITH_CONTENT 1

void
touch(char *fname, int optype)
{
  int fd, cfd;
  char* content = 0;

  switch(optype){
    case NULL_CONTENT:
      if((fd = open(fname, O_RDONLY)) < 0){
        cfd = open(fname, O_CREATE);
        close(cfd);
      }
      close(fd);
      break;
    
    case WITH_CONTENT:
      if((fd = open(fname, O_RDONLY)) > 0){
        unlink(fname);
      }
      close(fd);
      cfd = open(fname, O_CREATE | O_WRONLY);
      gets(content, LINE_BUF);
      write(cfd, content, strlen(content));
      close(cfd);
      break;
  }
}

int
main(int argc, char *argv[])
{
  if(argc < 2){
    printf(1, "touch: provide file name\n");
    exit();
  } else if(argc == 2){
    touch(argv[1], NULL_CONTENT);
    exit();
  } else if (argc == 3 && !strcmp(argv[1], "-w")){
    touch(argv[2], WITH_CONTENT);
    exit();
  } else {
    printf(1, "touch: too many arguments\n");
    exit();
  }     
}
