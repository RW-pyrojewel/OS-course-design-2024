#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#define NULL (int*) 0   //空指针（用于父进程等待）
#define MIN 2           //连续整数起点
#define MAX 35          //连续整数终点

//质数筛选器
void filter(int fd[])
{
  char buffer[MAX - MIN + 4]; //读缓冲区
  int prime = 0;              //第一个被筛出的数（必为质数）

  read(fd[0], buffer, sizeof(buffer));

  for (int i = 0; buffer[i]; i++) //寻找确定质数
    if (buffer[i] == '1')
    {
      prime = i;
      break;
    }

  if (prime)  //输入中存在确定的质数
  {
    printf("prime %d\n", prime);    //打印出确定质数
    for (int i = 0; buffer[i]; i++) //筛除确定质数的所有倍数
      if (!(i % prime))
        buffer[i] = '0';
    if (fork() > 0) //这一级子进程
    {
      write(fd[1], buffer, sizeof(buffer));
      close(fd[1]);
      wait(NULL);
    }
    else            //下一级子进程
    {
      filter(fd);
      close(fd[0]);
      close(fd[1]);
      wait(NULL);
    }
  }
  else
    exit(0);
}

int main(int argc, char* argv[])
{
  int fd[2];                    //用于父子进程通信的管道
  char info[MAX - MIN + 4];     //表示连续整数的信息（'1'代表可能为质数，'0'代表一定为合数）

  pipe(fd);  //创建管道

  //初始化整数信息
  info[0] = info[1] = '0';
  for (int i = 2; i <= MAX; i++)
    info[i] = '1';
  info[MAX - MIN + 3] = '\0';

  //调用fork创建子进程，返回值等于0说明在子进程中，大于0说明在父进程中
  if (fork() > 0)  //根进程
  {
    write(fd[1], info, sizeof(info));
    close(fd[1]);
    wait(NULL);
  }
  else             //一级子进程
  {
    filter(fd);
    close(fd[0]);
    close(fd[1]);
    wait(NULL);
  }

  exit(0);
}
