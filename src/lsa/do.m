path(path, '/scratch/mulhern/cbiexp_cvs/exif/analysis');

Findices = load('f.m');
Sindices = load('s.m');
Xsparse = load('X.m');
X = spconvert(Xsparse);
X = full(X);
[U,S,V] = svd(X);

