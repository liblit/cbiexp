function [groups, centers] = kmeans_fast (data, K, t),
  % function [groups, centers] = kmeans (data, K),
  %   data(i,j) = feature j of data point i,
  %   K = number of groups
  %   groups = vector of group assignments
  %   centers(i,j) = coordinate j of the center for group i

[M,N] = size(data);
groups = zeros (M,1);
ind = randperm (M);
centers = data(ind(1:K),:);
newcenters = zeros (K,N);
numchanged = N;
iter = 1;
while numchanged > 0 & iter < t,
  if mod(iter, 100) == 0, disp(iter); end;
  [newgroups, newcenters, numchanged] = kmeans_innerloop(data',groups',centers');
  groups = newgroups';
  centers = newcenters';
  iter = iter + 1;
end;
