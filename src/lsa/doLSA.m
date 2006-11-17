% Change directory to the actual working directory where the data is 
cd '/scratch/mulhern/cbiexp_cvs/exif/analysis'

%Load values for predicate * run matrix 
Xsparse = load('X.m');
X = spconvert(Xsparse);
clear Xsparse;

%when building an svd from a sparse matrix we can specify the number of 
%singular values we want. The more singular values we want, the longer this
%will take. I'm going to try making the number of singular values to be 1/2 the
%number of predicates.
nums = size(X); num_rows = nums(1,1); clear nums;
num_singulars = floor(num_rows/2); clear num_rows;
[U,S,V] = svds(X, num_singulars);
save decomposition.mat U S V X;
