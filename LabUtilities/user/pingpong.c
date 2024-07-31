#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#define BUFFER_SIZE 16  //读缓冲区大小
#define NULL (int*) 0   //空指针（用于父进程等待）

int main(int argc, char* argv[])
{
  int fd[2];               //用于父子进程通信的管道
  char* buffer[BUFFER_SIZE];  //读缓冲区

  pipe(fd);  //创建管道

  //调用fork创建子进程，返回值等于0说明在子进程中，大于0说明在父进程中
  if (fork() > 0)  //父进程
  {
    write(fd[1], "ping", 5);
    wait(NULL);
    read(fd[0], buffer, BUFFER_SIZE);
    printf("%d: received %s\n", getpid(), buffer);
    close(fd[0]);
  }
  else             //子进程
  {
    read(fd[0], buffer, BUFFER_SIZE);
    printf("%d: received %s\n", getpid(), buffer);
    write(fd[1], "pong", 5);
    close(fd[0]);
    close(fd[1]);
  }

  exit(0);
}
