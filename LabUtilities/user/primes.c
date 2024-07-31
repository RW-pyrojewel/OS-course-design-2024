#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#define NULL (int*) 0   //��ָ�루���ڸ����̵ȴ���
#define MIN 2           //�����������
#define MAX 35          //���������յ�

//����ɸѡ��
void filter(int fd[])
{
  char buffer[MAX - MIN + 4]; //��������
  int prime = 0;              //��һ����ɸ����������Ϊ������

  read(fd[0], buffer, sizeof(buffer));

  for (int i = 0; buffer[i]; i++) //Ѱ��ȷ������
    if (buffer[i] == '1')
    {
      prime = i;
      break;
    }

  if (prime)  //�����д���ȷ��������
  {
    printf("prime %d\n", prime);    //��ӡ��ȷ������
    for (int i = 0; buffer[i]; i++) //ɸ��ȷ�����������б���
      if (!(i % prime))
        buffer[i] = '0';
    if (fork() > 0) //��һ���ӽ���
    {
      write(fd[1], buffer, sizeof(buffer));
      close(fd[1]);
      wait(NULL);
    }
    else            //��һ���ӽ���
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
  int fd[2];                    //���ڸ��ӽ���ͨ�ŵĹܵ�
  char info[MAX - MIN + 4];     //��ʾ������������Ϣ��'1'�������Ϊ������'0'����һ��Ϊ������

  pipe(fd);  //�����ܵ�

  //��ʼ��������Ϣ
  info[0] = info[1] = '0';
  for (int i = 2; i <= MAX; i++)
    info[i] = '1';
  info[MAX - MIN + 3] = '\0';

  //����fork�����ӽ��̣�����ֵ����0˵�����ӽ����У�����0˵���ڸ�������
  if (fork() > 0)  //������
  {
    write(fd[1], info, sizeof(info));
    close(fd[1]);
    wait(NULL);
  }
  else             //һ���ӽ���
  {
    filter(fd);
    close(fd[0]);
    close(fd[1]);
    wait(NULL);
  }

  exit(0);
}
