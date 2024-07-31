#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"
#define LINESIZE 512  //行最大长度
#define NULL (void*)0 //空指针

int main(int argc, char* argv[]) {
  char* p[MAXARG];  //命令参数列表

  //检查参数数量
  if (argc + 1 > MAXARG)
    printf("Error: too many arguments.");

  //收集参数
  for (int i = 1; i < argc; i++)
    p[i - 1] = argv[i];
  p[argc - 1] = malloc(LINESIZE);
  p[argc] = NULL;

  //读行并执行
  while (gets(p[argc - 1], LINESIZE)) 
  {
    if (p[argc - 1][0] == 0) //已读完
      break;
    if (p[argc - 1][strlen(p[argc - 1]) - 1] == '\n') //替换换行符
      p[argc - 1][strlen(p[argc - 1]) - 1] = '\0';
    if (fork() > 0)  //父进程
      wait(NULL);
    else              //子进程
      exec(argv[1], p);
  }

  exit(0);
}
