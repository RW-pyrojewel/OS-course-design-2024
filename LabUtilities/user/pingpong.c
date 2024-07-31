#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#define BUFFER_SIZE 16  //����������С
#define NULL (int*) 0   //��ָ�루���ڸ����̵ȴ���

int main(int argc, char* argv[])
{
  int fd[2];               //���ڸ��ӽ���ͨ�ŵĹܵ�
  char* buffer[BUFFER_SIZE];  //��������

  pipe(fd);  //�����ܵ�

  //����fork�����ӽ��̣�����ֵ����0˵�����ӽ����У�����0˵���ڸ�������
  if (fork() > 0)  //������
  {
    write(fd[1], "ping", 5);
    wait(NULL);
    read(fd[0], buffer, BUFFER_SIZE);
    printf("%d: received %s\n", getpid(), buffer);
    close(fd[0]);
  }
  else             //�ӽ���
  {
    read(fd[0], buffer, BUFFER_SIZE);
    printf("%d: received %s\n", getpid(), buffer);
    write(fd[1], "pong", 5);
    close(fd[0]);
    close(fd[1]);
  }

  exit(0);
}
