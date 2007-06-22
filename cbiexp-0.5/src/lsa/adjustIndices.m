% X -- an index vector, adjusted so that it indexes correctly into a reduced
% matrix
% I -- the old index vector, a column vector
% V -- the selection vector, a row vector
function [X] = adjustIndices(I,V)
    X(V(end)) = 0;
    X(I,1) = ones(size(I,1),1); 
    X = find(X(V'));
return;
