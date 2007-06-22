/* ccrypt.c: functions for encrypting/decrypting a character stream */
/* $Id: ccrypt.c,v 1.1 2004/04/27 20:30:06 liblit Exp $ */

/* ccrypt implements a stream cipher based on the block cipher
   Rijndael, the candidate for the AES standard. */

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

#include "ccrypt.h"
#include "rijndael.h"
#include "io.h"
#include "main.h"
#include "unixcrypt.h"

#define MAGIC "c051"

/* hash a keystring into a 256-bit cryptographic random value. */

void hashstring(char *keystring, word32 hash[8]) {
  int i;
  roundkey rkk;
  word32 key[8];      /* rijndael key */

  for (i=0; i<8; i++) 
    key[i] = hash[i] = 0;
  
  do {
    for (i=0; i<32; i++) {
      if (*keystring != 0) {
	((word8 *)key)[i] ^= *keystring;
	keystring++;
      }
    }
    xrijndaelKeySched(key, 256, 256, &rkk);
    xrijndaelEncrypt(hash, &rkk);
  } while (*keystring != 0);
}

/* return a 256-bit value that is practically unique */
void make_nonce(word32 nonce[8]) {
  char acc[512], host[256];
  static int count=0;
  
  gethostname(host, 256);
  sprintf(acc, "%s, %d, %d, %d", host, (int)time(0), getpid(), count++);
  hashstring(acc, nonce); 
}

/* ---------------------------------------------------------------------- */

/* ccrypt error messages. These correspond to the error codes returned
   by the ccrypt functions. Note: the error code -1 corresponds to a
   system error, with errno set correctly */

const char *ccrypt_error(int st) {
  static const char *errstr[] = {
    /* 0 */  "no error",
    /* 1 */  "bad file format",
    /* 2 */  "key does not match",
  };
  if (st>=0) return errstr[st];
  if (st==-1) return strerror(errno);
  return "unknown error";
}  

/* encryption */
int ccencrypt(reader *r, writer *w, char *keyword) {
  word32 state[8];
  word8 *statec = (word8 *)state;
  roundkey rkk;

  word32 key[8];
  int i, c, cc;
  int st;

  /* generate the roundkey */
  hashstring(keyword, key);
  xrijndaelKeySched(key, 256, 256, &rkk);
  
  /* make a nonce */
  make_nonce(state);
  
  /* mark the nonce with a "magic number". */
  strncpy(statec, MAGIC, 4);

  /* encrypt the nonce with the given keyword */
  xrijndaelEncrypt(state, &rkk);
  
  /* write the seed (header) */
  for (i=0; i<32; i++) {
    st = w->bputc(statec[i], w);
    if (st<0) return -1;
  }

  /* encrypt the body of the stream */
  c = r->bgetc(r);
  while (c != EOF) {
    xrijndaelEncrypt(state, &rkk);

    for (i=0; i<32 && c != EOF; i++) {
      cc = c ^ statec[i];
      statec[i] = cc;
      st = w->bputc(cc, w);
      if (st<0) return -1;
      c = r->bgetc(r);
    }
  }
  if (errno) 
    return -1;

  return w->beof(w);
}

/* decryption */
int ccdecrypt(reader *r, writer *w, char *keyword) {
  word32 state[8];
  word32 seed[8];
  word8 *statec = (word8 *)state;
  word8 *seedchar = (word8 *)seed;
  roundkey rkk;

  word32 key[8];
  int i, c, cc;
  int st;
  
  /* generate the roundkey */
  hashstring(keyword, key);
  xrijndaelKeySched(key, 256, 256, &rkk);
  
  /* read the seed */
  for (i=0; i<32; i++) {
    seedchar[i] = statec[i] = cc = r->bgetc(r);
    if (cc == EOF) {
      if (errno) return -1;
      else return 1;  /* error: less than 32 characters in file */
    }
  }
  
  /* decrypt the seed with the given keyword */
  xrijndaelDecrypt(seed, &rkk);
  
  /* check the "magic number". */
  if (strncmp(seedchar, MAGIC, 4)) {
    if (!cmd.mismatch) {
      return 2;  /* error: wrong key or algorithm */
    }
    if (cmd.verbose>=0) {
      fprintf(stderr, "%s: warning: key does not match - decrypting anyway\n", cmd.name);
    }
  }
  
  /* decrypt the body of the stream */
  cc = r->bgetc(r);
  while (cc != EOF) {
    xrijndaelEncrypt(state, &rkk);

    for (i=0; i<32 && cc != EOF; i++) {
      c = cc ^ statec[i];
      statec[i] = cc;
      st = w->bputc(c, w);
      if (st<0) return -1;
      cc = r->bgetc(r);
    }
  }
  if (errno)
    return -1;

  return w->beof(w);
}

/* key change: decryption followed immediately by encryption.
   I found it to be easier to implement this separately than
   to do it via a pipeline. The procedure cckeychange is simply an 
   interleaving of ccdecrypt and ccencrypt. */ 

int cckeychange(reader *r, writer *w, char *keyword_in, char *keyword_out) {
  /* --decrypt-- */
  word32 state_in[8];
  word32 seed_in[8];
  word8 *statec_in = (word8 *)state_in;
  word8 *seedchar_in = (word8 *)seed_in;
  roundkey rkk_in;

  /* --encrypt-- */
  word32 state_out[8];
  word8 *statec_out = (word8 *)state_out;
  roundkey rkk_out;

  /* --shared-- */
  word32 key[8];
  int i, c, cc;
  int st;

  /* --decrypt-- */
  /* generate the roundkey */
  hashstring(keyword_in, key);
  xrijndaelKeySched(key, 256, 256, &rkk_in);

  /* --encrypt-- */
  /* generate the roundkey */
  hashstring(keyword_out, key);
  xrijndaelKeySched(key, 256, 256, &rkk_out);
  
  /* --decrypt-- */
  /* read the seed */
  for (i=0; i<32; i++) {
    seedchar_in[i] = statec_in[i] = cc = r->bgetc(r);
    if (cc == EOF) {
      if (errno) return -1;
      else return 1;  /* error: less than 32 characters in file */
    }
  }
  
  /* decrypt the seed with the given keyword */
  xrijndaelDecrypt(seed_in, &rkk_in);
  
  /* check the "magic number". */
  if (strncmp(seedchar_in, MAGIC, 4)) {
    return 2;  /* error: wrong key or algorithm */
  }

  /* key seems to match; generate new header */

  /* --encrypt-- */
  /* make a nonce */
  make_nonce(state_out);

  /* mark the nonce with a "magic number". */
  strncpy(statec_out, MAGIC, 4);

  /* encrypt the nonce with the given keyword */
  xrijndaelEncrypt(state_out, &rkk_out);

  /* write the seed (header) */
  for (i=0; i<32; i++) {
    st = w->bputc(statec_out[i], w);
    if (st<0) return -1;
  }
  
  /* --common, interleaved-- */
  /* transform the body of the stream */
  cc = r->bgetc(r);
  while (cc != EOF) {
    xrijndaelEncrypt(state_in, &rkk_in);
    xrijndaelEncrypt(state_out, &rkk_out);
    
    for (i=0; i<32 && cc != EOF; i++) {
      c = cc ^ statec_in[i];
      statec_in[i] = cc;
      cc = c ^ statec_out[i];
      statec_out[i] = cc;
      st = w->bputc(cc, w);
      if (st<0) return -1;
      cc = r->bgetc(r);
    }
  }
  if (errno)
    return -1;

  return w->beof(w);
}

/* ---------------------------------------------------------------------- */
/* specialize various encryption/decryption modes to files or streams */

int ccencrypt_file(int fd, char *filename, char *keyword) {
  readwriter *rw = new_file_readwriter(fd, filename);
  reader *r = (reader *)rw;
  writer *w = (writer *)rw;
  int st = ccencrypt(r, w, keyword);
  free(rw);
  return st;
}

int ccdecrypt_file(int fd, char *filename, char *keyword) {
  readwriter *rw = new_file_readwriter(fd, filename);
  reader *r = (reader *)rw;
  writer *w = (writer *)rw;
  int st = ccdecrypt(r, w, keyword);
  free(rw);
  return st;
}

int cckeychange_file(int fd, char *filename, char *key_in, char *key_out) {
  readwriter *rw = new_file_readwriter(fd, filename);
  reader *r = (reader *)rw;
  writer *w = (writer *)rw;
  int st = cckeychange(r, w, key_in, key_out);
  free(rw);
  return st;
}

int ccencrypt_streams(FILE *fin, FILE *fout, char *keyword) {
  reader *r = new_stream_reader(fin);
  writer *w = new_stream_writer(fout);
  int st = ccencrypt(r, w, keyword);
  free(r);
  free(w);
  return st;
}

int ccdecrypt_streams(FILE *fin, FILE *fout, char *keyword) {
  reader *r = new_stream_reader(fin);
  writer *w = new_stream_writer(fout);
  int st = ccdecrypt(r, w, keyword);
  free(r);
  free(w);
  return st;
}

int cckeychange_streams(FILE *fin, FILE *fout, char *key_in, char *key_out) {
  reader *r = new_stream_reader(fin);
  writer *w = new_stream_writer(fout);
  int st = cckeychange(r, w, key_in, key_out);
  free(r);
  free(w);
  return st;
}

