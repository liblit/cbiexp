#include "mex.h"
#include <math.h>

void kmeans_innerloop(double data[], double oldgroups[], double oldcenters[],
		      double newgroups[], double newcenters[], double numchanged[],
		      int M, int N, int K);

void kmeans_innerloop_sp(double data[], double oldgroups[], double oldcenters[],
		double newgroups[], double newcenters[], double numchanged[],
		int M, int N, int K,
		int ir[], int jc[], int nzmax);

/* [newgroups, newcenters, numchanged] = kmeans_innerloop(data, oldgroups, oldcenters); */
/* data is NxM, stored along columns, oldgroups is 1xM, oldcenters is NxK */
void mexFunction (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  int M,N,K;
  double *newgroups, *newcenters, *numchanged, *data, *oldgroups, *oldcenters;
  int nzmax;
  int *ir, *jc;

  /* check for num args */
  if (nrhs != 3) {
    mexErrMsgTxt ("Please input data and initial group and centers");
  } else if (nlhs != 3) {
    mexErrMsgTxt ("Please give three output arguments: newgroups, newcenters and numchanged");
  }

  data = mxGetPr(prhs[0]);
  oldgroups = mxGetPr(prhs[1]);
  oldcenters = mxGetPr (prhs[2]);
  M = mxGetN(prhs[0]);
  N = mxGetM(prhs[0]);
  K = mxGetN(prhs[2]);


  if (mxGetN(prhs[1]) != M || mxGetM(prhs[1]) != 1 || mxGetM(prhs[2]) != N) {
    mexErrMsgTxt ("Input vector dimension mismatch");
  }

  /* allocate space for newgroups */
  plhs[0] = mxCreateDoubleMatrix(1,M,mxREAL);
  newgroups = mxGetPr(plhs[0]);

  /* allocate space for newcenters */
  plhs[1] = mxCreateDoubleMatrix(N,K,mxREAL);
  newcenters = mxGetPr(plhs[1]);

  /* get pointer to numchanged */
  plhs[2] = mxCreateDoubleMatrix(1,1,mxREAL);
  numchanged = mxGetPr(plhs[2]);

  if (mxIsSparse(prhs[0])) {
	nzmax = mxGetNzmax(prhs[0]);
	ir = mxGetIr(prhs[0]);
	jc = mxGetJc(prhs[0]);
	kmeans_innerloop_sp (data, oldgroups, oldcenters, newgroups, newcenters,numchanged,
			M,N,K,ir,jc,nzmax);
  }
  else 
	kmeans_innerloop(data, oldgroups, oldcenters, newgroups, newcenters, numchanged, M, N, K);

}

void kmeans_innerloop(double data[], double oldgroups[], double oldcenters[],
		      double newgroups[], double newcenters[], double numchanged[],
		      int M, int N, int K)
{
  int i, j, k;

/*
  for (i = 0; i < M; i++) {
    for (j = 0; j < N; j++) {
      printf ("data(%d, %d) = %g\n", i+1, j+1, data[j*M+i]);
    }
    }
*/

  numchanged[0] = 0;

  for (i = 0; i < M; i++) {
    double mindist = 1e20;
    int index = -1;
    for (k = 0; k < K; k++) {
      double newmindist = 0;
      for (j = 0; j < N; j++) {
	double diff = data[i*N+j] - oldcenters[k*N+j];
	newmindist += diff * diff;
      }
      if (newmindist < mindist) {
	index = k + 1;
	mindist = newmindist;
      }
    }
    newgroups[i] = index;
    if (newgroups[i] != oldgroups[i]) { numchanged[0] += 1; }
  }

  for (k = 0; k < K; k++) {
    int numgroupk = 0;
    for (i = 0; i < M; i++) {
      if (newgroups[i] == k+1) {
	numgroupk += 1;
	for (j = 0; j < N; j++) {
	  newcenters[k*N+j] += data[i*N+j];
	}
      }
    }
    for (j = 0; j < N; j++) {
      newcenters[k*N+j] = newcenters[k*N+j] / numgroupk;
    }
  }
}

void kmeans_innerloop_sp(double data[], double oldgroups[], double oldcenters[],
		double newgroups[], double newcenters[], double numchanged[],
		int M, int N, int K,
		int ir[], int jc[], int nzmax)
{
  int i, j, k, r;

  numchanged[0] = 0;

  for (i = 0; i < M; i++) {
    int r1 = jc[i];
    int r2 = jc[i+1]-1;
    double mindist = 1e20;
    int index = -1;
    for (k = 0; k < K; k++) {
      double newmindist = 0;
      for (r = r1; r <= r2; r++) {
	double diff;
        j = ir[r];
        if (r >= nzmax) { mexErrMsgTxt ("index too big!\n"); }
	diff = data[r] - oldcenters[k*N+j];
	newmindist += diff * diff;
      }
      if (newmindist < mindist) {
	index = k + 1;
	mindist = newmindist;
      }
    }
    newgroups[i] = index;
    if (newgroups[i] != oldgroups[i]) { numchanged[0] += 1; }
  }

  for (k = 0; k < K; k++) {
    int numgroupk = 0;
    for (i = 0; i < M; i++) {
      if (newgroups[i] == k+1) {
	int r1 = jc[i];
	int r2 = jc[i+1]-1;
	numgroupk += 1;
	for (r = r1; r <= r2; r++) {
	  j = ir[r];
	  newcenters[k*N+j] += data[r];
	}
      }
    }
    for (j = 0; j < N; j++) {
      newcenters[k*N+j] = newcenters[k*N+j] / numgroupk;
    }
  }
}
