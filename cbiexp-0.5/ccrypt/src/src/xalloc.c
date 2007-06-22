/* xalloc.c: safe dynamic allocation */
/* $Id: xalloc.c,v 1.1 2004/04/27 20:30:06 liblit Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xalloc.h"

/* safe malloc */
void *xalloc(size_t size, char *myname) {
  void *p = malloc(size);
  if (p==NULL) {
    fprintf(stderr, "%s: out of memory\n", myname);
    exit(2);
  }
  return p;
}

/* safe realloc */
void *xrealloc(void *p, size_t size, char *myname) {
  p = realloc(p, size);
  if (p==NULL) {
    fprintf(stderr, "%s: out of memory\n", myname);
    exit(2);
  }
  return p;
}

#define INITSIZE 32

/* read an allocated line from input stream. Strip trailing
   newline. On end of file, return NULL if zero characters read, or
   the read characters otherwise. Note: for some operating systems
   this is pointless, for instance, Solaris limits line length on the
   terminal to 1025, including the trailing newline character. */

char *xreadline(FILE *fin, char *myname) {
  int buflen = INITSIZE;

  char *buf = xalloc(buflen, myname);
  char *res, *nl;

  res = fgets(buf, INITSIZE, fin);
  if (res==NULL) {
    free(buf);
    return NULL;
  }
  nl = strchr(buf, '\n');
  while (nl == NULL) {
    int oldbuflen = buflen;
    buflen <<= 1;
    buf = xrealloc(buf, buflen, myname);
    res = fgets(buf+oldbuflen-1, buflen-oldbuflen+1, fin);
    if (res==NULL) return buf;
    nl = strchr(buf+oldbuflen-1, '\n');
  }
  *nl = 0;
  return buf;
}


