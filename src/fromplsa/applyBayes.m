function Py_x = applyBayes(Px_y,Py,Px)
    Py_x = (diag(invertNonZeros(Px)) * Px_y * diag(Py))';
return;
