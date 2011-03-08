function ENoSelfLoops = removeSelfLoops(E)
% Returns adjacency matrix after having removed self-transitions

  numNodes = size(E, 1);
  E(eye(numNodes) == 1) = zeros(numNodes, 1); % remove self-transitions
  ENoSelfLoops = E;

end
