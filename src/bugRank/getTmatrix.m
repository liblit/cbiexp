function T = getTmatrix(E, R, beta, threshold)
% subroutine used to compute the transition effects when following
% edges.
% E -- the adjacency matrix
% R -- the scores used as input for computing the edge based effects
% beta -- probability mass spent exploring non-scored nodes
% threshold -- decides how much probability mass should be divided evenly
% amongst elements of R which are zero. Defaults to zero

  % DEFAULTS
  if ~exist('threshold', 'var')
    threshold = 0;
  end


  R = R(:); % ensure column vector
  R = R / sum(R); % normalize R
  R(~R, :) = threshold/size(R,1);
  R = R / sum(R); % normalize R

  I = ones(size(R));
  teleportSource = findDangling(E);
  scoredNodes = (R ~= 0)'; % is a row-vector

  E = addTeleportationEdges(E, teleportSource, scoredNodes);

  noNeighWithScore = (~sum(E(:, scoredNodes), 2))'; % row-vector
  hasScore = scoredNodes';
  
  T = (E .* ((hasScore .* R) * I')) ./ (I * (E * R)');
  T(isnan(T)) = 0;
  num = beta * noNeighWithScore + (1 - beta) * I';
  denom = (E * I)';
  T = (beta * T) + (E .* (I * (num ./ denom)));
  
end
