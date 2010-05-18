function Z = invertNonZeros(V)
    Z = spfun(inline('1./x'),V);
return;
