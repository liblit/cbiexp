/* traverse.c: functions for traversing through a list of files,
   optionally recursing through directory structure, and doing
   whatever action is required for encrypting/decrypting files in the
   various modes */
/* $Id: traverse.c,v 1.1 2004/04/27 20:30:06 liblit Exp $ */ 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <utime.h>
#include <unistd.h>

#include "xalloc.h"
#include "main.h"
#include "traverse.h"
#include "ccrypt.h"
#include "unixcrypt.h"

/* ---------------------------------------------------------------------- */
/* an "object" for keeping track of a list of inodes that we have seen */

/* state: a list of inode/device pairs. numinodes is number of nodes
   in the list, and sizeinodes is its allocated size */
ino_t *inodes = NULL;
dev_t *devs = NULL;
int numinodes = 0;
int sizeinodes = 0;

/* reset the list */
void reset_inodes(void) {
  if (inodes!=NULL) {
    free(inodes);
    free(devs);
  }
  numinodes = 0;
  inodes = NULL;
  devs = NULL;
}

/* return 0 the first time this is called with a particular
   inode/device pair, and 1 each successive time */
int known_inode(ino_t ino, dev_t dev) {
  int i;

  /* have we already seen this inode/device pair? */
  for (i=0; i<numinodes; i++) {
    if (inodes[i] == ino && devs[i] == dev) {
      return 1;
    }
  }

  /* if not, add it to the list */
  if (inodes==NULL) {
    sizeinodes = 100;
    inodes = xalloc(sizeinodes*sizeof(ino_t), cmd.name);
    devs = xalloc(sizeinodes*sizeof(dev_t), cmd.name);
  }
  if (numinodes >= sizeinodes) {
    sizeinodes += 100;
    inodes = xrealloc(inodes, sizeinodes*sizeof(ino_t), cmd.name);
    devs = xrealloc(devs, sizeinodes*sizeof(dev_t), cmd.name);
  }
  inodes[numinodes] = ino;
  devs[numinodes] = dev;
  numinodes++;
  return 0;
}

/* ---------------------------------------------------------------------- */

/* return 1 if filename ends in, but is not equal to, suffix. */
int has_suffix(char *filename, char *suffix) {
  int flen = strlen(filename);
  int slen = strlen(suffix);
  return flen>slen && strcmp(filename+flen-slen, suffix)==0;
}

/* add suffix to filename */

char *add_suffix(char *filename, char *suffix) {
  static char *outfile = NULL;
  int flen = strlen(filename);
  int slen = strlen(suffix);

  outfile = xrealloc(outfile, flen+slen+1, cmd.name);
  strncpy (outfile, filename, flen);
  strncpy (outfile+flen, suffix, slen+1);
  return outfile;
}

/* remove suffix from filename */

char *remove_suffix(char *filename, char *suffix) {
  static char *outfile = NULL;
  int flen = strlen(filename);
  int slen = strlen(suffix);

  if (suffix[0]==0 || !has_suffix(filename, suffix))
    return filename;
  outfile = xrealloc(outfile, flen-slen+1, cmd.name);
  strncpy (outfile, filename, flen-slen);
  outfile[flen-slen] = 0;
  return outfile;
}

/* ---------------------------------------------------------------------- */

/* read a yes/no response from the user */
int prompt(void) {
  char *line;
  FILE *fin;

  fin = fopen("/dev/tty", "r");
  if (fin==NULL) {
    fin = stdin;
  }
  
  line = xreadline(fin, cmd.name);
#ifndef FAULT_PROMPT_XREADLINE_NULL_CHECK
  if (line == NULL)
    return 0;
#endif
  return (!strcmp(line, "y") || !strcmp(line, "yes"));
}

/* check whether named file exists */
int file_exists(char *filename) {
  struct stat buf;
  int st;

  st = lstat(filename, &buf);

  if (st) 
    return 0;
  else 
    return 1;
}

/* (re)set attributes of filename to those in bufp, where
   possible. Fail quietly if this is not possible */

void set_attributes(char *filename, struct stat *bufp) {
  struct utimbuf ut = {bufp->st_atime, bufp->st_mtime};

  utime(filename, &ut);
  chown(filename, bufp->st_uid, bufp->st_gid);
  chmod(filename, bufp->st_mode);
}

/* ---------------------------------------------------------------------- */

/* this function is called to act on a file if cmd.mode is ENCRYPT,
   DECRYPT, or KEYCHANGE */
int crypt_filename(char *filename) {
  int fd;
  int res;
  
  /* open file */
#ifdef __CYGWIN__
  fd = open(filename, O_RDWR | O_BINARY);
#else
  fd = open(filename, O_RDWR);
#endif
  if (fd == -1) {
    perror(filename);
    return 0;
  }
  
  /* crypt */
  switch (cmd.mode) {   /* note: can't be CAT or UNIXCRYPT */

  case ENCRYPT: default:
    if (cmd.verbose>0)
      fprintf(stderr, "Encrypting %s\n", filename);
    res = ccencrypt_file(fd, filename, cmd.keyword);
    break;
    
  case DECRYPT:
    if (cmd.verbose>0)
      fprintf(stderr, "Decrypting %s\n", filename);
    res = ccdecrypt_file(fd, filename, cmd.keyword);
    break;
    
  case KEYCHANGE:
    if (cmd.verbose>0)
      fprintf(stderr, "Changing key for %s\n", filename);
    res = cckeychange_file(fd, filename, cmd.keyword, cmd.keyword2);
    break;
    
  }    
  
  /* close file */
  close(fd);

  return res;
}

/* this function is called to act on a file if cmd.mode is CAT or
   UNIXCRYPT */
int crypt_cat(char *infile) {
  FILE *fin;
  int res;
  
  /* open file */
  fin = fopen(infile, "rb");
  if (fin == NULL) {
    perror(infile);
    return 0;
  }

  /* crypt */

  switch (cmd.mode) {

  case CAT: default:
    if (cmd.verbose>0) {
      fprintf(stderr, "Decrypting %s\n", infile);
      fflush(stderr);
    }
    res = ccdecrypt_streams(fin, stdout, cmd.keyword);
    break;

  case UNIXCRYPT:
    if (cmd.verbose>0) {
      fprintf(stderr, "Decrypting %s\n", infile);
      fflush(stderr);
    }
    res = unixcrypt_streams(fin, stdout, cmd.keyword);
    break;

  }
  fflush(stdout);

  /* close file */
  fclose(fin);

  return res;
}

/* ---------------------------------------------------------------------- */

void file_action(char *filename) {
  struct stat buf;
  int st;
  int link = 0;
  char buffer[strlen(filename)+strlen(cmd.suffix)+1];
  char *outfile;

  st = lstat(filename, &buf);

  if (st) {
    int orig_errno = errno;
    char *orig_filename = filename;

    /* if file didn't exist and decrypting, try if suffixed file exists */
    if (errno==ENOENT 
	&& (cmd.mode==DECRYPT || cmd.mode==CAT || cmd.mode==KEYCHANGE 
	    || cmd.mode==UNIXCRYPT) 
	&& cmd.suffix[0]!=0) {
      strcpy(buffer, filename);
      strcat(buffer, cmd.suffix);
      filename=buffer;
      st = lstat(buffer, &buf);
    }
    if (st) {
      errno = orig_errno;
      perror(orig_filename);
      return;
    }
  }
  
  /* if link following is enabled, follow links */
  if (cmd.symlinks && S_ISLNK(buf.st_mode)) {
    link = 1;
    st = stat(filename, &buf);
    /* if there was an error, print it but don't return. */
    if (st) {
      perror(filename);
    }
  }

  /* if file is not a regular file, skip */
  if (!st && S_ISLNK(buf.st_mode)) {
    if (cmd.verbose>=0)
      fprintf(stderr, "%s: %s: is a symbolic link -- ignored\n", cmd.name, filename);
    return;
  }
  if (!st && !S_ISREG(buf.st_mode)) {
    if (cmd.verbose>=0) 
      fprintf(stderr, "%s: %s: is not a regular file -- ignored\n", cmd.name, 
	      filename);
    return;
  }
  
  /* now we have a regular file, and we have followed a link if
     appropriate. Or the link exists but the file does not. */
  
  /* do the mode-dependent encryption/decryption */

  switch (cmd.mode) {

  case ENCRYPT: case DECRYPT: case KEYCHANGE: /* overwrite mode */

    /* determine outfile name */
    switch (cmd.mode) {
    case ENCRYPT: default:
      outfile = add_suffix(filename, cmd.suffix);
      break;
    case DECRYPT:
      outfile = remove_suffix(filename, cmd.suffix);
      break;
    case KEYCHANGE:
      outfile = filename;
      break;
    }
    
    /* if outfile exists and cmd.force is not set, prompt whether to
       overwrite */
    if (!cmd.force && strcmp(filename, outfile) && 
	file_exists(outfile)) {
      fprintf(stderr, "%s: %s already exists; overwrite (y or n)? ", cmd.name, 
	      outfile);
      fflush(stderr);
      if (prompt()==0) {
	fprintf(stderr, "Not overwritten.\n");
	return;
      }
    }
  
    /* crypt file unless already done so */
    if (!st) {
      if (known_inode(buf.st_ino, buf.st_dev)) {
	if (cmd.verbose>0)
	  fprintf(stderr, "Already visited inode %s.\n", filename);
      } else {
	if (buf.st_nlink>1 && cmd.verbose>=0)
	  fprintf(stderr, "%s: warning: %s has %d links\n", cmd.name, 
		  filename, buf.st_nlink);
	st = crypt_filename(filename);
	set_attributes(filename, &buf);
	if (st>0) {
	  fprintf(stderr, "%s: %s: %s -- unchanged\n", cmd.name, filename,
		  ccrypt_error(st));
	  break;
	} else if (st<0) {
	  fprintf(stderr, "%s: %s: %s\n", cmd.name, filename,
		  ccrypt_error(st));
	  exit(3);
	}
      }
    }
    
    /* rename file if necessary */
    if (strcmp(filename, outfile)) {
      st = rename(filename, outfile);
      if (st) {
	fprintf(stderr, "%s: could not rename %s as %s: ", cmd.name, 
		filename, outfile);
	perror("");
      }
    }

    if (sigint_flag) {  /* SIGINT received while crypting - delayed exit */
      exit(6);
    }
    break;
    
  case CAT: case UNIXCRYPT: default: 
    if (!st) {
      st = crypt_cat(filename);
      if (st>0) {
	fprintf(stderr, "%s: %s: %s -- ignored\n", cmd.name, filename,
		ccrypt_error(st));
      } else if (st<0) {
	fprintf(stderr, "%s: %s: %s\n", cmd.name, filename,
		ccrypt_error(st));
	exit(3);
      }
    }
    break;
    
  }
}

/* ---------------------------------------------------------------------- */

/* read a whole directory. This is because we change directory entries
   while traversing the directory; this could otherwise lead to
   strange behavior on Solaris. After done with the file list, it
   should be freed with free_filelist. */

int get_filelist(char *dirname, char*** filelistp, int *countp) {
  DIR *dir;
  struct dirent *dirent;
  char **filelist = NULL;
  int count = 0;

  dir = opendir(dirname);
  if (dir==NULL) {
    perror(dirname);
    *filelistp = NULL;
    *countp = 0;
    return 0;
  }
  
  while ((dirent = readdir(dir)) != NULL) {
    if (strcmp(dirent->d_name, "..")!=0 && strcmp(dirent->d_name, ".")!=0) {
      char *strbuf = xalloc(strlen(dirname)+strlen(dirent->d_name)+2, cmd.name);
      strcpy (strbuf, dirname);
      strcat (strbuf, "/");
      strcat (strbuf, dirent->d_name);
      filelist = xrealloc(filelist, (count+1)*sizeof(char *), cmd.name);
      filelist[count] = strbuf;
      count++;
    }
  }
  
  closedir(dir);

  *filelistp = filelist;
  *countp = count;
  return count;
}

void free_filelist(char** filelist, int count) {
  int i;

  for (i=0; i<count; i++)
    free(filelist[i]);
  free(filelist);
}

/* ---------------------------------------------------------------------- */
/* if filename is a directory or a symlink to a directory, traverse
   recursively if appropriate or issue warning and do nothing. In all
   other cases, call action with the filename. Do this even if the
   file does not exist. Descend into directories if recursive>=1, and
   follow symlinks if recursive==2. */

void traverse_file(char *filename) {
  struct stat buf;
  int st;
  int link = 0;
  
  st = lstat(filename, &buf);
  if (!st && S_ISLNK(buf.st_mode)) {  /* is a symbolic link */
    link = 1;
    st = stat(filename, &buf);
  }
  if (st || !S_ISDIR(buf.st_mode)) {
    file_action(filename);
    return;
  }
  
  /* is a directory */
  if (cmd.recursive<=1 && link==1) { /* ignore link */
    if (cmd.verbose>=0)
      fprintf(stderr, "%s: %s: directory is a symbolic link -- ignored\n", cmd.name, 
	      filename);
    return;
  } else if (cmd.recursive==0) {  /* ignore */
    if (cmd.verbose>=0)
      fprintf(stderr, "%s: %s: is a directory -- ignored\n", cmd.name, filename);
    return;
  } else if (known_inode(buf.st_ino, buf.st_dev)) { /* already traversed */
    if (cmd.verbose>0)
      fprintf(stderr, "Already visited directory %s -- skipped.\n", filename);
    return;
  }

  /* recursively traverse directory */
  {
    char **filelist;
    int count;

    get_filelist(filename, &filelist, &count);
    traverse_files(filelist, count);
    free_filelist(filelist, count);
  }
}

/* same as traverse_file, except go through a list of files. */
void traverse_files(char **filelist, int count) {
  while (count > 0) {
    traverse_file(*filelist);
    ++filelist, --count;
  }
}
