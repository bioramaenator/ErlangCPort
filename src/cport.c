#include <stdio.h>
#include <malloc.h>
#include <errno.h>
#include <string.h>

typedef unsigned char byte;

int read_cmd(byte *buf)
{
  int len;
  //fprintf(stderr, "try read\r\n");
  if (read_exact(buf, 4) != 4)
    {
      //fprintf(stderr, "bad: %s\r\n", strerror(errno));
      return(-1);
    }

  //fprintf(stderr, "ll: %d, %d, %d, %d\r\n", buf[0], buf[1], buf[2], buf[3]);
  len = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
  //fprintf(stderr, "#len: %d\r\n", len);
  return read_exact(buf, len);
}

int write_cmd(byte *buf, int len)
{
  byte li[4];
  li[0] = (len >> 24) & 0xff;
  li[1] = (len >> 16) & 0xff;
  li[2] = (len >> 8)  & 0xff;
  li[3] = len  & 0xff;
  write_exact(&li, 4);
  
  //fprintf(stderr, "last b: %d\r\n", li[3]);
  //fprintf(stderr, "actualy reply len: %d\r\n", len);
  return write_exact(buf, len);
}

int read_exact(byte *buf, int len)
{
  int i, got=0;
  do {
    //fprintf(stderr, "read %d bytes\r\n", len - got);
    if ((i = read(0, buf+got, len-got)) <= 0)
      {
	//fprintf(stderr, "bad1: %s, return %d\r\n", strerror(errno), i);
	return(i);
      }
    got += i;
  } while (got<len);
  //fprintf(stderr, "len: %d\r\n", len);
  return len;
}

int write_exact(byte *buf, int len)
{
  int i, wrote = 0;
  do {
    if ((i = write(1, buf+wrote, len-wrote)) < 0)
      return (i);
    wrote += i;
  } while (wrote<len);
  return len;
}

int main() {
  int fn, arg, res;
  byte *buf = malloc(31457280); // 30 mb
  int len = 0;
  while ((len = read_cmd(buf)) > 0) {
    write_cmd(buf, len);
  }
}
