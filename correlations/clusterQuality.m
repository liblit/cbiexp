function score = clusterQuality(groups, centers, Y, K),

score = 0;
for k = 1:K,
  groupk = find(groups==k);
  if isempty(groupk), continue; end;
  nk = length(groupk);
  avedist = sum(sum((Y(groupk,:) - repmat(centers(k,:),nk,1)).^2))/nk;
  score = score + avedist;
end;
score = score / K;
