function Px = marginalize(Px_y,Py)
    Px = sum(Px_y * diag(Py),2);
return;
