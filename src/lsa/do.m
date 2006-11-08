path(path, '/scratch/mulhern/cbiexp_cvs/exif/analysis');

Findices = load('f.m');
Sindices = load('s.m');
Xsparse = load('X.m');
X = spconvert(Xsparse);
clear Xsparse;
[U,S,V] = svds(X, 30);
singulars = diag(S);
Uk = U(:,1:2);
Sk = S(1:2,1:2); 
Vk = V(:,1:2);
