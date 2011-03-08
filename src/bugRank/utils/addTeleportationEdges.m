function E = addTeleportationEdges(E, teleportSource, teleportDest,areEdgesDirected)
% Utility function to add edges to an adjacency matrix E. Has been
% refactored, because this will need to be rewritten if we want to take
% advantage of the sparsity of E.
%
% ARGUMENTS:
% E -- The adjacency matrix
% teleportSource -- Vector of indices where new edges need to be inserted
% teleportDest -- Vector of indices which mark the destination of the newly
% added edges
% areEdgesDirects -- Controls whether the edges directed or not
%
% RETURNS:
% Modified matrix E

  if ~exist('areEdgesDirected', 'var')
    areEdgesDirected = true;
  end

  E(teleportSource, :) = (ones(nnz(teleportSource), 1) * teleportDest) | E(teleportSource, :);
  if ~areEdgesDirected 
    E(:, teleportSource) = teleportDest' * ones(1, nnz(teleportSource)) + E(:, teleportSource);
  end
end
