#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

//��ʽ��Ŀ¼��
char* fmtname(char* path)
{
  static char buf[DIRSIZ + 1];
  char* p;

  // Find first character after last slash.
  for (p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if (strlen(p) >= DIRSIZ)
    return p;

  memmove(buf, p, strlen(p));
  memset(buf + strlen(p), '\0', DIRSIZ - strlen(p));
  return buf;
}

//find��������
void find(char* path, char* target)
{
  char buf[512], * p;
  int fd;
  struct dirent de;
  struct stat st;
  char* fmt;

  if ((fd = open(path, 0)) < 0) 
  {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if (fstat(fd, &st) < 0) 
  {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  //����Ŀ¼����ƥ����ֱ�����
  if (!strcmp(fmtname(path), target))
    printf("%s\n", path);

  //�ݹ�������Ŀ¼
  switch (st.type) 
  {
    case T_FILE:
      break;
    case T_DIR:
      if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) 
      {
        printf("find: path too long\n");
        break;
      }
      strcpy(buf, path);
      p = buf + strlen(buf);
      *p++ = '/';
      while (read(fd, &de, sizeof(de)) == sizeof(de)) 
      {
        if (de.inum == 0)
          continue;
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        if (stat(buf, &st) < 0)
        {
          printf("find: cannot stat %s\n", buf);
          continue;
        }
        fmt = fmtname(buf);
        if (fmt[0] != '.' || (fmt[1] && (fmt[1] != '.' || fmt[2])))
          find(buf, target);
      }
      break;
  }

  close(fd);
}

int main(int argc, char* argv[])
{
  if (argc < 2)       //�޲�
    printf("Error: at least one parameter required.\n");
  else if (argc == 2) //һ�Σ�Ĭ���Ե�ǰ·��Ϊ��Ŀ¼
    find(".", argv[1]);
  else if (argc == 3) //���Σ���ָ����Ŀ¼��Ŀ������
    find(argv[1], argv[2]);
  else                //���
    printf("Error: too many parameters.\n");

  exit(0);
}
