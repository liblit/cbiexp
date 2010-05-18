function N = normalizeColumns(X)
    C = sum(X,1); 
    N = X * diag(full(invertNonZeros(C)));
return;
