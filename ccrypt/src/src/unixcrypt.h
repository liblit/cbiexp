/* unixcrypt.c: functions to simulate old "unix crypt" program */
/* $Id: unixcrypt.h,v 1.1 2004/04/27 20:30:06 liblit Exp $ */

/* WARNING: do not use this software for encryption! The encryption
   provided by this program has been broken and is not secure. Only
   use this software to decrypt existing data. */

#ifndef __UNIXCRYPT_H
#define __UNIXCRYPT_H

#include <stdio.h>

/* state of the encryption engine */
struct unixcrypt_state_s {
  char box1[0x100];
  char box2[0x100];
  char box3[0x100];
  int j;
  int k;
};
typedef struct unixcrypt_state_s unixcrypt_state;

void unixcrypt_init(unixcrypt_state *st, char *key);
int unixcrypt_char(unixcrypt_state *st, int c);
int unixcrypt_file(int fd, char *filename, char *keyword);
int unixcrypt_streams(FILE *fin, FILE *fout, char *keyword);

#endif /* __UNIXCRYPT_H */

