#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char* argv[])
{
  int legal = 1;  //�����Ϸ��Ա�ʶ
  char* p;        //�����ַ�������ָ��

  //�Ϸ���sleep����Ӧ��ֻ����һ�����β��������argcӦ��Ϊ2
  if (argc < 2)
    printf("Error: one parameter required.\n");
  else if (argc > 2)
    printf("Error: too many parameters.\n");
  else
  {
    for (p = argv[1]; *p; p++)
      if (*p < '0' || *p > '9') //������Ϊ����
      {
        legal = 0;
        break;
      }
    if (legal)  //�������Ϸ��������atoiת��Ϊ����
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
