#include "mex.h"
#include <math.h>

/*
 * This function reads fills a sparse matrix directly from data, jc, and ir
 * files.  The input format is 
 * SP = readsp(FILEROOT, NZMAX, M, N)
 * The output SP will be a sparse matrix with M rows and N columns and
 * nzmax non-zero elements
 * FILEROOT is the name of the file holding the data vector in ascii text
 * format, one element per line;
 * FILEROOTir should be the file holding the ir vector
 * FILEROOTjc should be the file holding the jc vector
 * The calling program can use FILEEROOTmeta to figure out nzmax, M, and N,
 * if the file exists.
 */

/*
 * Note about the Matlab sparse matrix format:
 * A Matlab sparse matrix of contains three vectors: data, ir, and jc.
 *   data - an array of length nzmax containing all the non-zero data
 *          elements of the matrix, stacked column-wise, i.e., the
 *          non-zero elements of column 1 come before those of column 2
 *	    before those of column 3, etc.
 *   ir - an integer array of length nzmax, where
 *        ir[i] is the row index of data[i]
 *   jc - an integer array of length ncols+1, where
 *        jc[j] is the index into data and ir of the first non-zero element
 *        in column j
 */

void mexFunction (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  int M, N, nzmax, buflen, i;
  char buf[1024];
  char fnroot[1024];
  double *dataX;
  int *datair, *datajc;
  FILE *datafd, *irfd, *jcfd;

  if (nrhs != 4) {
    mexErrMsgTxt ("Please input sparse matrix file name root, nzmax, M, and N");
  }
  else if (nlhs != 1) {
    mexErrMsgTxt ("Please allow one output argument");
  }

  /* Input must be a string */
  if (mxIsChar(prhs[0]) != 1)
    mexErrMsgTxt("Input must be a string.");

  /* get length of string */
  buflen = mxGetM(prhs[0]) * mxGetN(prhs[0]) + 1;
  if (mxGetString(prhs[0], fnroot, buflen))
    mexErrMsgTxt("Not enough space, input truncated");

  nzmax = *(mxGetPr(prhs[1]));
  M = *(mxGetPr(prhs[2]));
  N = *(mxGetPr(prhs[3]));

  printf ("Maximum number of non-zero elements is %d ", nzmax);
  printf ("M is %d and N is %d\n", M, N);

  plhs[0] = mxCreateSparse(M, N, nzmax, 0);
  dataX = mxGetPr(plhs[0]);
  datair = mxGetIr(plhs[0]);
  datajc = mxGetJc(plhs[0]);

  sprintf(buf, "%s", fnroot);
  printf ("Reading sparse matrix from %s\n", buf);
  datafd = fopen(buf, "r");
  if (!datafd)
    mexErrMsgTxt("Cannot open data file");

  sprintf (buf, "%sir", fnroot);
  irfd = fopen(buf, "r");
  if (!irfd) mexErrMsgTxt("Cannot open ir file");
 
  sprintf (buf, "%sjc", fnroot);
  jcfd = fopen(buf, "r");
  if (!jcfd) mexErrMsgTxt("Cannot open jc file");

  for (i = 0; i < nzmax; i++) {
    fscanf(datafd, "%lf", dataX);
    if (feof(datafd))
      mexErrMsgTxt("Expecting more data");
    dataX++;

    fscanf(irfd, "%d", datair);
    if (feof(irfd))
      mexErrMsgTxt("Expecting more ir");
    datair++;
  }

  for (i = 0; i <= N; i++) {
    fscanf (jcfd, "%d", datajc);
    if (feof(jcfd)) 
      mexErrMsgTxt("Expecting more jc");
    datajc++;
  }

  fclose (datafd);
  fclose (irfd);
  fclose (jcfd);
}
