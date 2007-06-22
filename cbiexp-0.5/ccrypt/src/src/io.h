/* $Id: io.h,v 1.1 2004/04/27 20:30:06 liblit Exp $ */

#ifndef __IO_H
#define __IO_H

#include <stdio.h>

/* reader: abstract class of readable things */
typedef struct reader {
  int (*bgetc)(struct reader *this);
} reader;

/* writer: abstract class of writable things */
typedef struct writer {
  int (*dummy)();   /* place holder so that a reader can also be a writer */
  int (*bputc)(int c, struct writer *this);
  int (*beof)(struct writer *this);
} writer;

/* NOTE: beof not only flushes any output buffers, but also truncates
   the file in overwrite mode. Thus it should only be called if there
   wasn't an error! */

/* readwriter: a reader and a writer at the same time */
typedef struct readwriter {
  int (*bgetc)(struct reader *this);
  int (*bputc)(int c, struct writer *this);
  int (*beof)(struct writer *this);
} readwriter;

/* constructors of readers / writers from streams or file descriptors */
reader *new_stream_reader(FILE *in);
writer *new_stream_writer(FILE *out);
reader *new_reader(int fd);
writer *new_writer(int fd);

/* file_readwriter: read/write a single file, avoiding conflicts */
readwriter *new_file_readwriter(int fd, char *filename);

/* pipe_readwriter: make a new pipeline */
readwriter *new_pipe_readwriter();


#endif /* __IO_H */

