/* readkey.c: read secret key phrase from terminal */
/* $Id: readkey.c,v 1.1 2004/04/27 20:30:06 liblit Exp $ */

#include <termios.h>
#include <stdio.h>
#include <stdlib.h>

#include "xalloc.h"

/* read key from /dev/tty */
char *readkey(char *prompt, char *myname) {
  char *line;
  FILE *fin;
  struct termios tio, saved_tio;

  fin = fopen("/dev/tty", "r");
  if (fin==NULL) {
    fprintf(stderr, "%s: cannot open /dev/tty\n", myname);
    exit(2);
  }

  fprintf(stderr, "%s", prompt);
  fflush(stderr);

  /* disable echo */
  tcgetattr(fileno(fin), &tio);
  saved_tio = tio;
  tio.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHONL);
  tcsetattr(fileno(fin), TCSANOW, &tio);

  /* read key */
  line = xreadline(fin, myname);

  /* restore echo, print newline, close file */
  tcsetattr(fileno(fin), TCSANOW, &saved_tio);
  fprintf(stderr, "\n");
  fflush(stderr);
  fclose(fin);

  return line;
}
