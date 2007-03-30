% S - the result of minDiffs, sorted in descending order, the largest of the
% minimum differences will be at the left. Size is 1 * m.
% I - index vector for the sort 
% X - the matrix, n * m
% row - a row index in X. The row at this index is compared with all the other
% rows in X.
function [S,I] = sortMinDiffs(X, index)
    X = circshift(X, [(1 - index) 0]);    
    Xcur = X(1,:);
    Xrest = X(2:end,:); 
    [S,I] = sort(min(repmat(Xcur, size(Xrest,1), 1) - Xrest, [], 1),2, 'descend'); 
return;
