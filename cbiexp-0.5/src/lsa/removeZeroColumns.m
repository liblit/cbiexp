% X -- original matrix with zero columns removed
% I -- indices corresponding to non-zero columns, a row vector
function [X,I] = removeZeroColumns(X)
    I = find(sum(X,1) > 0);; 
    X = X(:,I);
return;
