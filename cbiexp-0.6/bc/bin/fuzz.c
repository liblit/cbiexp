#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>


static void insist(int invariant)
{
  if (!invariant)
    {
      fputs("usage: fuzz <seed> <megs> <outfile>\n", stderr);
      exit(2);
    }
}


static unsigned parse(const char *text)
{
  char *remainder;
  unsigned long result;

  errno = 0;
  result = strtoul(text, &remainder, 0);

  insist(!errno);
  insist(remainder != 0);
  insist(*remainder == '\0');
  insist(remainder != text);

  return result;
}


int main(int argc, char *argv[])
{
  size_t bytes;
  int fd;
  int32_t *map;
  unsigned words, word;
  
  insist(argc == 4);
  srand48(parse(argv[1]));
  bytes = parse(argv[2]) * 1024 * 1024;
  fd = open(argv[3], O_RDWR | O_CREAT, 0666);

  ftruncate(fd, bytes);
  map = (int32_t *) mmap(0, bytes, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  insist(map != 0);
  insist((intptr_t) map != -1);
  
  words = bytes / sizeof(int32_t);
  for (word = 0; word < words; ++word)
    map[word] = mrand48();

  munmap(map, bytes);
  close(fd);
  return 0;
}
