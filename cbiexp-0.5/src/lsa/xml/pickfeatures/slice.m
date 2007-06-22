% get features indices for a selected region
% X is a vector
function I = slice(X, upper, lower, left, right)
    H = find(X > lower & X < upper);    
    V = find(H > left & H < right);
    I = H(V); 
return;
