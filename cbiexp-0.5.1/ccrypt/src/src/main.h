/* user interface for ccrypt: casual encryption and decryption for files */
/* $Id: main.h,v 1.1 2004/04/27 20:30:06 liblit Exp $ */ 

#ifndef __MAIN_H
#define __MAIN_H

/* modes */
#define ENCRYPT   0
#define DECRYPT   1
#define KEYCHANGE 2
#define CAT       3
#define UNIXCRYPT 4

/* structure to hold command-line */
typedef struct {
  char *name;        /* invocation name: "NAME", "NAMECAT", etc */
  int verbose;       /* -1=quiet, 0=normal, 1=verbose */
  int debug;    
  char *keyword;
  char *keyword2;    /* when changing keys: new key */
  int mode;          /* ENCRYPT, DECRYPT, KEYCHANGE, CAT, UNIXCRYPT */
  int filter;        /* running as a filter? */
  char *suffix;
  char *prompt;
  int recursive;     /* 0=non-recursive, 1=directories, 2=dirs and symlinks */
  int symlinks;      /* operate on files that are symbolic links? */
  int force;         /* overwrite existing files without asking? */
  int mismatch;      /* allow decryption with non-matching key? */
  char **infiles;    /* list of filenames */
  int count;         /* number filenames */
  char *keyfile;     /* file to read key(s) from */
  int timid;         /* prompt twice for destructive encryption keys? */
} cmdline;

extern cmdline cmd;
extern int sigint_flag;

#endif /* __MAIN_H */
