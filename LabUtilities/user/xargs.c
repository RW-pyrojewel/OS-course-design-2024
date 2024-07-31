#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"
#define LINESIZE 512  //����󳤶�
#define NULL (void*)0 //��ָ��

int main(int argc, char* argv[]) {
  char* p[MAXARG];  //��������б�

  //����������
  if (argc + 1 > MAXARG)
    printf("Error: too many arguments.");

  //�ռ�����
  for (int i = 1; i < argc; i++)
    p[i - 1] = argv[i];
  p[argc - 1] = malloc(LINESIZE);
  p[argc] = NULL;

  //���в�ִ��
  while (gets(p[argc - 1], LINESIZE)) 
  {
    if (p[argc - 1][0] == 0) //�Ѷ���
      break;
    if (p[argc - 1][strlen(p[argc - 1]) - 1] == '\n') //�滻���з�
      p[argc - 1][strlen(p[argc - 1]) - 1] = '\0';
    if (fork() > 0)  //������
      wait(NULL);
    else              //�ӽ���
      exec(argv[1], p);
  }

  exit(0);
}
