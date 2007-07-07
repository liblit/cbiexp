/* maketables.c: create lookup tables for Rijndael cipher */
/* $Id: maketables.c,v 1.1 2004/04/27 20:30:06 liblit Exp $ */

#include <stdio.h>

typedef unsigned char		word8;	
typedef unsigned short		word16;	
typedef unsigned long		word32;

word8 exp[256];
word8 log[256];
int explog_init = 0;

word8 sbox[256];
word8 sibox[256];
int sbox_init = 0;

word8 L[256];

/* ---------------------------------------------------------------------- */

/* multiply two elements a and b of GF(2^8) modul0 0x11b */
word8 multiply(word8 a, word8 b) {
  unsigned int acc = a;
  unsigned int res = 0;
  while (b!=0) {    /* invariant: acc*b + res = const (mod 0x11b)*/
    if (b&1) 
      res ^= acc;
    acc<<=1;
    b>>=1;          /* invariant holds */
    if (acc>=0x100)
      acc ^= 0x11b; /* invariant holds */
  }
  return res;
}  

void makeexplog() {
  int i, p;  /* p = 3^i in GF(2^8) */
  
  for (i=0, p=1; i<256; i++, p=multiply(3,p)) {
    exp[i] = p;
    log[p] = i % 255;
  }
  log[0]=0;
  explog_init = 1;
}
  
/* ---------------------------------------------------------------------- */

/* find the inverse of a in GF(2^8) or 0 */
word8 xinv(word8 a) {
  if (!explog_init)
    makeexplog();
  if (a) return exp[255-log[a]];
  else return 0;
}

/* apply an affine transformation over GF(2) */
word8 xaff(word8 a) {
  unsigned int acc = a;
  acc^=acc<<1;
  acc^=acc<<2;
  acc<<=1;
  acc^=a;
  return acc ^ acc>>8;
}

void makesboxes() {
  int i, p;

  for (i=0; i<256; i++) {
    p = 99^xaff(xinv(i));
    sbox[i] = p;
    sibox[p] = i;
  }
  sbox_init = 1;
}

/* ---------------------------------------------------------------------- */

word32 multrot2113(int rot, word8 m) {
  word8 r[4];

  r[rot]         = multiply(2,m);
  r[(rot+1) % 4] = m;
  r[(rot+2) % 4] = m;
  r[(rot+3) % 4] = multiply(3,m);
  return *(word32 *)r;
}

word32 multrote9db(int rot, word8 m) {
  word8 r[4];

  r[rot]         = multiply(0xe,m);
  r[(rot+1) % 4] = multiply(0x9,m);
  r[(rot+2) % 4] = multiply(0xd,m);
  r[(rot+3) % 4] = multiply(0xb,m);
  return *(word32 *)r;
}

/* ---------------------------------------------------------------------- */

void printbox8(word8 b[256], char *name) {
  int i;

  printf("word8 %s[256] = {", name);
  for (i=0; i<256; i++) {
    if (i%16 == 0) printf("\n  ");
    printf("%3d,", b[i]);
  }
  printf("\n};\n\n");
}

/* ---------------------------------------------------------------------- */

void makeL() {
  int i;

  for (i=0; i<256; i++) {
    L[i] = ((i & 0x55) << 1) ^ ((i & 0xAA) >> 1) ^ (i & 0xAA);
  }
}

/* ---------------------------------------------------------------------- */

int main() {
  int rot, m, i, rcon;
  word8 r[4];

  printf("#include \"rijndael.h\"\n\n");

  printf("word8 M0b[4][256][4] = {");
  for (rot=0; rot<4; rot++) {
    printf("\n {");
    for (m=0; m<256; m++) {
      if (m%4 == 0) printf("\n  ");
      *(word32 *)r = multrot2113(rot,m);
      printf("{%3d,%3d,%3d,%3d}, ", r[0], r[1], r[2], r[3]);
    }
    printf("\n },");
  }
  printf("\n};\n\n");

  printf("word8 M1b[4][256][4] = {");
  for (rot=0; rot<4; rot++) {
    printf("\n {");
    for (m=0; m<256; m++) {
      if (m%4 == 0) printf("\n  ");
      *(word32 *)r = multrote9db(rot,m);
      printf("{%3d,%3d,%3d,%3d}, ", r[0], r[1], r[2], r[3]);
    }
    printf("\n },");
  }
  printf("\n};\n\n");

  printf("word32 (*M0)[256] = (word32 (*)[256])M0b;\n");
  printf("word32 (*M1)[256] = (word32 (*)[256])M1b;\n");

  rcon = 0x01;
  printf("word8 xrcon[30] = {");
  for (i=0; i<30; i++) {
    if (i%15==0) printf("\n  ");
    printf("0x%02x,", rcon);
    rcon = multiply(2,rcon);
  }
  printf("\n};\n\n");

  makeexplog();
  makesboxes();
  
  printbox8(log, "xLogtable");
  printbox8(exp, "xAlogtable");
  printbox8(sbox, "xS");
  printbox8(sibox, "xSi");

  makeL();
  printbox8(L, "L");

  return 0;
}
