function danglingNodes = findDangling(E)
% Function to find dangling nodes in the graph whose adjacency matrix is
% given by E.
%
% Returns:
% danglingNodes -- column vector

    E = removeSelfLoops(E);
    danglingNodes = sum(E, 2) == 0;
end
