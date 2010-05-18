%% run spec cluster
%% need from environment: A (or rho and sigma)

K = 5:20;
iters = 1000;
N = size(rho,1);
sigma = 0.25;
A = exp(rho/(2*sigma^2));
for i = 1:N, A (i,i) = 0; end;
disp('Computing L');
Dsqrt = diag(1./sqrt(sum(A,2)));
L = Dsqrt*A*Dsqrt;
L = (L+L')/2;
disp('Doing eigendecomposition');
[V,lambda] = eigs(L,max(K), 'la');
[l, ind] = sort(diag(lambda));
ind = flipud(ind);
l = flipud(l);
V = V(:,ind);

allgroups = zeros(N, length(K));
allquals = zeros(1, length(K));
allquals(:) = inf;
for i = 1:length(K),
  disp('Normalizing');
  Y = V(:,1:K(i))./repmat(sqrt(sum(V(:,1:K(i)).^2,2)),1,K(i));

  for t = 1:10,
    disp('Running kmeans');
    [groups, centers] = kmeans_fast(Y,K(i), iters);
    s = clusterQuality(groups, centers, Y, K(i));
    if s <= allquals(i),
      allquals(i) = s;
      allgroups(:,i) = groups;
    end;
  end;

end;

[val,i] = min(allquals);
bestK = K(i);
bestgroups = allgroups(:,i);
