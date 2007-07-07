/* $Id: io.c,v 1.1 2004/04/27 20:30:06 liblit Exp $ */
/* an object-oriented interface to input and output, providing an
   abstraction which allows us to treat a read-writable file like a pair
   of streams. */

#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#include "io.h"
#include "main.h"
#include "xalloc.h"

/* ---------------------------------------------------------------------- */
/* readers and writers from streams */

typedef struct stream_reader {
  int (*bgetc)(reader *this);
  FILE *in;
} stream_reader;

typedef struct stream_writer {
  int (*dummy)();
  int (*bputc)(int c, writer *this);
  int (*beof)(writer *this);
  FILE *out;
} stream_writer;

/* methods */

int sgetc(reader *this) {
  stream_reader *b = (stream_reader *)this;
  errno = 0;
  return fgetc(b->in);
}

int sputc(int c, writer *this) {
  stream_writer *b = (stream_writer *)this;
  int st = fputc(c, b->out);
  if (st<0) return st;
  return 0;
}

int seof(writer *this) {
  return 0;
}

/* constructors */

reader *new_stream_reader(FILE *in) {
  stream_reader *r = xalloc(sizeof(stream_reader), cmd.name);

  r->bgetc = sgetc;
  r->in = in;
  return (reader *)r;
}

writer *new_stream_writer(FILE *out) {
  stream_writer *w = xalloc(sizeof(stream_writer), cmd.name);

  w->bputc = sputc;
  w->beof = seof;
  w->out = out;
  return (writer *)w;
}

/* ---------------------------------------------------------------------- */
/* file_readwriter */

/* we use the following abstraction when reading and writing the same
   file simultaneously: there is a read pointer rp and an write
   pointer wp, which contain the index of the next character to be
   read, resp., written, in the file. Moreover, let p = rp-wp.  If we
   suppose that both pointers are non-decreasing, and that rp-wp is
   always non-nengative, then read and write operations don't
   interfere. We use a buffered write, but wp refers to the physical
   write pointer, so writing to the buffer does not increase wp. */

#define BUFFERSIZE (1000)

typedef struct file_readwriter {
  int (*bgetc)(reader *this);
  int (*bputc)(int c, writer *this);
  int (*beof)(writer *this);

  char buffer[BUFFERSIZE];
  int first, next, size;  /* Invariants: 
			     0 <= first, next < BUFFERSIZE
			     0 <= size <= BUFFERSIZE 
			     next-first == size (mod BUFFERSIZE) */
  char inbuffer[BUFFERSIZE];
  int innext, insize;
  int p;
  int fd;  /* file descriptor */
  char *filename;
} file_readwriter;

/* private methods */

/* write at most n bytes from buffer - write as many bytes as
   possible. Return <0 on i/o error and set errno, else 0. */
int bflush(file_readwriter *b, int n) {
  int st;

  if (n > b->size)
    n = b->size;
  
  st = lseek(b->fd, -(b->p), SEEK_CUR);  /* set point to write position */
  if (st<0) return -1;

  if (b->first + n > BUFFERSIZE) {
    st = write(b->fd, &b->buffer[b->first], BUFFERSIZE - b->first);
    if (st<0) return -1;
    b->size -= BUFFERSIZE - b->first;
    (b->p) -= BUFFERSIZE - b->first;
    n -= BUFFERSIZE - b->first;
    b->first = 0;
  }

  st = write(b->fd, &b->buffer[b->first], n);
  if (st<0) return -1;
  b->size -= n;
  b->first = (b->first + n) % BUFFERSIZE;
  (b->p) -= n;
  n = 0;
  st = lseek(b->fd, b->p, SEEK_CUR);   /* reset point to read position */
  if (st<0) 
    return -1;
  else
    return 0;
}

/* methods */

/* read from the buffer. return EOF on end of file or error (set errno=0 
   if not an error)  */
int bgetc(reader *this) {
  int st;
  file_readwriter *b = (file_readwriter *)this;
  unsigned char c;

  if (b->insize == 0) {     /* inbuffer is empty, must read from file */
    b->innext = 0;
    st = b->insize = read(b->fd, b->inbuffer, BUFFERSIZE);
    if (st<0) return EOF;
    b->p += b->insize;
    if (b->insize == 0) {   /* end of file */
      errno = 0;
      return EOF;
    }
  }

  c = b->inbuffer[b->innext];
  b->innext++;
  b->insize--;
  return c;
}  

/* buffered write of c to file, guaranteeing the constraint for p.
   Returns the new p. Buffer size is at least 32. Return <0 on i/o error 
   and set errno, return >0 on other error, else return 0. */
int bputc(int c, writer *this) {
  int st;
  file_readwriter *b = (file_readwriter *)this;
  
  if (b->size >= BUFFERSIZE) {     /* buffer full - must write to file */
    st = bflush(b, b->size < b->p ? b->size : b->p);
    if (st<0) return -1;
  }

  if (b->size < BUFFERSIZE) {  /* we can now write to buffer */
    b->buffer[b->next] = c;
    (b->size)++;
    b->next = (b->next + 1) % BUFFERSIZE;
  } else {                  /* an error: we were unable to shrink buffer */
    return 1;
  }
  return 0;
}

/* flush buffer and truncate file. No more reads or writes after this. 
   Return <0 on error and set errno, else 0. */
int beof(writer *this) {
  file_readwriter *b = (file_readwriter *)this;
  int st;

  st = bflush(b, b->size);
  if (st<0) return -1;

  /* truncate file to where it's been written */
  st = ftruncate(b->fd, lseek(b->fd, - (b->p), SEEK_CUR));
  if (st<0) return -1;

  return 0;
}

/* constructor */

readwriter *new_file_readwriter(int fd, char *filename) {
  file_readwriter *b = xalloc(sizeof(file_readwriter), cmd.name);

  b->bgetc = bgetc;
  b->bputc = bputc;
  b->beof = beof;

  b->first = 0;
  b->next = 0;
  b->size = 0;
  b->innext = 0;
  b->insize = 0;
  b->size = 0;
  b->p = 0;
  b->fd = fd;
  b->filename = filename;

  return (readwriter *)b;
}

