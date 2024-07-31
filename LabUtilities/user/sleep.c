#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char* argv[])
{
  int legal = 1;  //参数合法性标识
  char* p;        //参数字符串遍历指针

  //合法的sleep调用应当只包含一个整形参数，因此argc应当为2
  if (argc < 2)
    printf("Error: one parameter required.\n");
  else if (argc > 2)
    printf("Error: too many parameters.\n");
  else
  {
    for (p = argv[1]; *p; p++)
      if (*p < '0' || *p > '9') //参数不为整数
      {
        legal = 0;
        break;
      }
    if (legal)  //若参数合法，则调用atoi转换为整数
      sleep(atoi(argv[1]));
    else
    {
      printf("Error: illegal parameter ");
      printf(argv[1]);
      printf(".\n");
    }
  }

  exit(0);
}
