function printspeccluster(cormat, groups, K, outdir, fndiff, plistfn, porderfn),
%% to be run after we obtain the spectral clusters 
%% group membership is held in groups
%% number of clusters is K

fn = sprintf('%s/predlist-speccluster%s.txt', outdir, fndiff);
fd = fopen(fn, 'w');

preds = load(plistfn);
pincrsort = load(porderfn);
N = length(preds);
predrank = zeros(N,1);
for i = 1:N,
  predrank(i) = find(pincrsort==preds(i));
end;

for k = 1:K,
  groupk = find(groups == k);

  if (isempty(groupk)),
    fprintf (fd, '\n');
    continue;
  end;

  %% rank them by increase score
  grank = predrank(groupk);
  [val, inds] = sort(grank);

  p1 = groupk(inds(1));
  num1 = preds(groupk(inds(1)));
  fprintf (fd, '%d: ', num1);
  for i = inds(2:end)',
    p2 = groupk(i);
    num2 = preds(p2);
    fprintf(fd, '%d %.6g ', num2, cormat(p1,p2));
  end;
  fprintf (fd, '\n');
end;
fclose (fd);
