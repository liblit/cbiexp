function printClusterSummary (Af, groups, K, path, suffix),

runs = zeros(size(Af,1), K);
for k = 1:K,
  groupk = find(groups==k);
  runs(:,k) = sum(Af(:,groupk),2);
end;

fn = sprintf('%s/cluster-summary-%s.txt', path, suffix);
fd = fopen(fn, 'w');
if fd == 0,
  error('Invalid file descriptor');
end;
for k = 1:K,
  inds = find(runs(:,k));
  for i = 1:length(inds),
    fprintf(fd, '%d ', inds(i));
  end;
  fprintf (fd, '\n');
end;
fclose (fd);
