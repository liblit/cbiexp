#include "mex.h"
#include <math.h>

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

  plhs[0] = mxCreateSparse(N, M, nzmax, 0);
  dataX = mxGetPr(plhs[0]);
  datair = mxGetIr(plhs[0]);
  datajc = mxGetJc(plhs[0]);

  /* note, the input format has row and column reversed,
   * so the output matrix will be a transpose of 
   * the input matrix
   */
  
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

    fscanf(jcfd, "%d", datair);
    if (feof(jcfd))
      mexErrMsgTxt("Expecting more jc");
    datair++;
  }

  for (i = 0; i <= M; i++) {
    fscanf (irfd, "%d", datajc);
    if (feof(irfd)) 
      mexErrMsgTxt("Expecting more ir");
    datajc++;
  }

  fclose (datafd);
  fclose (irfd);
  fclose (jcfd);
}
