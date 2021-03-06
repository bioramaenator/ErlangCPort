#include <stdio.h>
#include <malloc.h>
#include <errno.h>
#include <string.h>

typedef unsigned char byte;

int read_cmd(byte *buf)
{
  int len;
  if (read_exact(buf, 4) != 4)
      return(-1);

  len = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
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

  return write_exact(buf, len);
}

int read_exact(byte *buf, int len)
{
  int i, got=0;
  do {
    if ((i = read(0, buf+got, len-got)) <= 0)
      {
	return(i);
      }
    got += i;
  } while (got<len);
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
