/* rijndael.c - optimized version of the Rijndeal cipher */
/* $Id: rijndael.c,v 1.1 2004/04/27 20:30:06 liblit Exp $ */

/* derived from original source: rijndael-alg-ref.c   v2.0   August '99
 * Reference ANSI C code for NIST competition
 * authors: Paulo Barreto
 *          Vincent Rijmen
 */

#include <stdio.h>
#include <stdlib.h>

#include "rijndael.h"

static int xshifts[3][2][4] = {
  0,1,2,3,
  0,3,2,1,
  
  0,1,2,3,
  0,5,4,3,

  0,1,3,4,
  0,7,5,4,
};

/* Exor corresponding text input and round key input bytes */
/* the result is written to res, which can be the same as a */
void xKeyAddition(word8 res[MAXBC][4], word8 a[MAXBC][4], 
		  word32 rk[MAXBC], word8 BC) {
  int j;
  
  for(j = 0; j < BC; j++)
    *(word32 *)res[j] = *(word32 *)a[j] ^ rk[j];
}

/* shift rows a, return result in res. This avoids having to copy a
   tmp array back to a. res must not be a. */
void xShiftRow(word8 res[MAXBC][4], word8 a[MAXBC][4], int shift[4], 
	       word8 BC)
{
  /* Row 0 remains unchanged
   * The other three rows are shifted a variable amount
   */
  int i, j;
  int s;
  
  for (j=0; j < BC; j++) {
    res[j][0] = a[j][0];
  }
  for(i = 1; i < 4; i++) {
    s = shift[i];
    for(j = 0; j < BC; j++) 
      res[j][i] = a[(j + s) % BC][i];
  }
}

void xSubstitution(word8 a[MAXBC][4], word8 box[256], word8 BC) {
  /* Replace every byte of the input by the byte at that place
   * in the nonlinear S-box
   */
  int i, j;
  
  for(i = 0; i < 4; i++)
    for(j = 0; j < BC; j++) 
      a[j][i] = box[a[j][i]] ;
}

/* profiling shows that the ccrypt program spends about 50% of its
   time in the function xShiftSubst. Splitting the inner "for"
   statement into two parts - versus using the expensive "%" modulo
   operation, makes this function about 44% faster, thereby making the
   entire program about 28% faster. With -O3 optimization, the time
   savings are even more dramatic - ccrypt runs between 55% and 65%
   faster on most platforms. */

/* do ShiftRow and Substitution together. res must not be a. */
void xShiftSubst(word8 res[MAXBC][4], word8 a[MAXBC][4], int shift[4], 
		 word8 BC, word8 box[256]) {
  int i, j;
  int s;
  
  for (j=0; j < BC; j++) {
    res[j][0] = box[a[j][0]];
  }
  for(i = 1; i < 4; i++) {
    s = shift[i];
    for(j = 0; j < BC-s; j++) 
      res[j][i] = box[a[(j + s)][i]];
    for(j = BC-s; j < BC; j++) 
      res[j][i] = box[a[(j + s) - BC][i]];
  }
}

/* Mix the four bytes of every column in a linear way */
/* the result is written to res, which may equal a */
void xMixColumn(word8 res[MAXBC][4], word8 a[MAXBC][4], word8 BC) {
  int j;
  word32 b;

  for(j = 0; j < BC; j++) {
    b =  M0[0][a[j][0]];
    b ^= M0[1][a[j][1]];
    b ^= M0[2][a[j][2]];
    b ^= M0[3][a[j][3]];
    *(word32 *)res[j] = b;
  }
}

/* do MixColumn and KeyAddition together */
void xMixAdd(word8 res[MAXBC][4], word8 a[MAXBC][4], word32 rk[MAXBC], 
	     word8 BC) {
  int j;
  word32 b;

  for(j = 0; j < BC; j++) {
    b =  M0[0][a[j][0]];
    b ^= M0[1][a[j][1]];
    b ^= M0[2][a[j][2]];
    b ^= M0[3][a[j][3]];
    b ^= rk[j];
    *(word32 *)res[j] = b;
  }
}

/* Mix the four bytes of every column in a linear way
 * This is the opposite operation of xMixColumn */
/* the result is written to res, which may equal a */
void xInvMixColumn(word8 res[MAXBC][4], word8 a[MAXBC][4], word8 BC) {
  int j;
  word32 b;
  
  for(j = 0; j < BC; j++) {
    b =  M1[0][a[j][0]];
    b ^= M1[1][a[j][1]];
    b ^= M1[2][a[j][2]];
    b ^= M1[3][a[j][3]];
    *(word32 *)res[j] = b;
  }
}

/* do KeyAddition and InvMixColumn together */
void xAddInvMix(word8 res[MAXBC][4], word8 a[MAXBC][4], word32 rk[MAXBC], 
		  word8 BC) {
  int j;
  word32 b;
  
  for(j = 0; j < BC; j++) {
    *(word32 *)a[j] = *(word32 *)a[j] ^ rk[j];
    b =  M1[0][a[j][0]];
    b ^= M1[1][a[j][1]];
    b ^= M1[2][a[j][2]];
    b ^= M1[3][a[j][3]];
    *(word32 *)res[j] = b;
  }
}

int xrijndaelKeySched (word32 key[], int keyBits, int blockBits, 
		       roundkey *rkk) {
  /* Calculate the necessary round keys
   * The number of calculations depends on keyBits and blockBits */
  int KC, BC, ROUNDS;
  int i, j, t, rconpointer = 0;
  word8 (*k)[4] = (word8 (*)[4])key;
  word32 *k32 = (word32 *)key;

  switch (keyBits) {
  case 128: KC = 4; break;
  case 192: KC = 6; break;
  case 256: KC = 8; break;
  default : return -1;
  }

  switch (blockBits) {
  case 128: BC = 4; break;
  case 192: BC = 6; break;
  case 256: BC = 8; break;
  default : return -2;
  }

  ROUNDS = KC>BC ? KC+6 : BC+6;

  t = 0;
  /* copy values into round key array */
  for(j = 0; (j < KC) && (t < (ROUNDS+1)*BC); j++, t++)
    rkk->rk[t] = k32[j];
  
  while (t < (ROUNDS+1)*BC) { /* while not enough round key material */
    /* calculate new values */
    for(i = 0; i < 4; i++)
      k[0][i] ^= xS[k[KC-1][(i+1)%4]];
    k[0][0] ^= xrcon[rconpointer++];
    
    if (KC != 8)
      for(j = 1; j < KC; j++)
	k32[j] ^= k32[j-1];
    else {
      for(j = 1; j < 4; j++)
	k32[j] ^= k32[j-1];
      for(i = 0; i < 4; i++) 
	k[4][i] ^= xS[k[3][i]];
      for(j = 5; j < 8; j++)
	k32[j] ^= k32[j-1];
    }
    /* copy values into round key array */
    for(j = 0; (j < KC) && (t < (ROUNDS+1)*BC); j++, t++)
      rkk->rk[t] = k32[j];
  }		

  /* make roundkey structure */
  rkk->BC = BC;
  rkk->KC = KC;
  rkk->ROUNDS = ROUNDS;
  for (i=0; i<2; i++) 
    for (j=0; j<4; j++)
      rkk->shift[i][j] = xshifts[(BC-4) >> 1][i][j];
  
  return 0;
}
      
/* Encryption of one block. */

void xrijndaelEncrypt (word32 block[], roundkey *rkk)
{
  word8 (*a)[4] = (word8 (*)[4])block;
  word8 a2[MAXBC][4]; /* hold intermediate result */
  int r;

  int *shift = rkk->shift[0];
  int BC = rkk->BC;
  int ROUNDS = rkk->ROUNDS;
  word32 *rp = rkk->rk;

  /* begin with a key addition */
  xKeyAddition(a,a,rp,BC);
  rp += BC;
  
  /* ROUNDS-1 ordinary rounds */
  for(r = 1; r < ROUNDS; r++) {
    xShiftSubst(a2,a,shift,BC,xS);
    xMixAdd(a,a2,rp,BC);
    rp += BC;
  }

  /* Last round is special: there is no xMixColumn */
  xShiftSubst(a2,a,shift,BC,xS);
  xKeyAddition(a,a2,rp,BC);
}   

void xrijndaelDecrypt (word32 block[], roundkey *rkk)
{
  word8 (*a)[4] = (word8 (*)[4])block;
  word8 a2[MAXBC][4]; /* Hold intermediate result */
  int r;

  int *shift = rkk->shift[1];
  int BC = rkk->BC;
  int ROUNDS = rkk->ROUNDS;
  word32 *rp = rkk->rk + ROUNDS*BC;
  
  /* To decrypt: apply the inverse operations of the encrypt routine,
   *             in opposite order
   * 
   * (xKeyAddition is an involution: it 's equal to its inverse)
   * (the inverse of xSubstitution with table S is xSubstitution with the 
   * inverse table of S)
   * (the inverse of xShiftRow is xShiftRow over a suitable distance)
   */
  
  /* First the special round:
   *   without xInvMixColumn
   *   with extra xKeyAddition
   */
  xKeyAddition(a2,a,rp,BC);
  xShiftSubst(a,a2,shift,BC,xSi);   
  rp -= BC;

  /* ROUNDS-1 ordinary rounds
   */
  for(r = ROUNDS-1; r > 0; r--) {
    xKeyAddition(a,a,rp,BC);
    xInvMixColumn(a2,a,BC);
    xShiftSubst(a,a2,shift,BC,xSi);  
    rp -= BC;
  }
  
  /* End with the extra key addition
   */
  
  xKeyAddition(a,a,rp,BC);    
}

