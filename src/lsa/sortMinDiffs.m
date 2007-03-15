% S - the result of minDiffs, sorted in descending order, the largest of the
% minimum differences will be at the left. 
% I - index vector for the sort 
function [S,I] = sortMinDiffs(X)
    [X] = minDiffs(X);
    [S,I] = sort(X,2,'descend'); 
return;


% X - the ith row is a vector of the minium difference between the ith row of
% Y and all the other rows of i. This minimum difference is per column. The
% minimum difference may be less than zero.
% Formally: X(i,j) = min(Y(i,j) - Y(i',j) for i' <> i
% Y - any matrix 
function [X] = minDiffs(Y) 
   rows = size(Y,1);
   for i = 1:rows;
       X(i,:) = min(repmat(Y(1,:), rows - 1, 1) - Y(2:end,:), [], 1);
       Y = circshift(Y, [1 0]); 
   end
return;
