function Z = getZmatrix(E,R,alpha,beta,threshold)
% subroutine used to compute the transition effects when following
% edges.
% E -- the adjacency matrix
% R -- the scores used as input for computing the edge based effects
% alpha -- probability following links (as opposed to random teleportation)
% beta -- probability mass spent exploring non-scored nodes
% threshold -- decides how much probability mass should be divided evenly
% amongst elements of R which are zero. Defaults to zero

  T = getTmatrix(E,R,beta,threshold);
  num = (1 - alpha) * (R * I');
  denom = I' * R;
  Z = (alpha * T) + (num / denom);
    
end
