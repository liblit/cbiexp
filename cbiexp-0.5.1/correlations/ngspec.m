function [Y, groups] = ngspec(A,K);
%% implements the Ng-Jordan-Weiss spectral clustering algorithm

disp('Computing L');
Dsqrt = diag(1./sqrt(sum(A,2)));
L = Dsqrt*A*Dsqrt;
disp('Doing eigendecomposition');
[V,lambda] = eigs(L,K);
disp('Normalizing');
Y = V./repmat(sqrt(sum(V.^2,2)),1,K);
disp('Running kmeans');
[groups, centers] = kmeans_fast(Y,K, inf);
