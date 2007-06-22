/* ccrypt.h: functions for encrypting/decrypting a character stream */
/* $Id: ccrypt.h,v 1.1 2004/04/27 20:30:06 liblit Exp $ */

#ifndef __CCRYPT_H
#define __CCRYPT_H

#include "io.h"

/* encrypt or decrypt a stream */
int ccencrypt_streams(FILE *fin, FILE *fout, char *keyword);
int ccdecrypt_streams(FILE *fin, FILE *fout, char *keyword);
int cckeychange_streams(FILE *fin, FILE *fout, char *key_in, char *key_out);

/* destructively encrypt or decrypt a file. Mode must be read-write */
int ccencrypt_file(int fd, char *filename, char *keyword);
int ccdecrypt_file(int fd, char *filename, char *keyword);
int cckeychange_file(int fd, char *filename, char *key_in, char *key_out);

/* error messages corresponding to return codes of the ccrypt functions */
const char *ccrypt_error(int st);

#endif /* __CCRYPT_H */

