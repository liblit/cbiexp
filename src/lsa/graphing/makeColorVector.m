% V -- the color row vector with indices, indices are distributed on a color map
% M -- matrix with rows corresponding to the property we want to color 
function V = makeColorVector(M)
    if not(isempty(find(sum(M,1) > 1)));
        error('M matrix is incorrectly formatted');
    end;

    [I,J] = find(M); 
    V(1,size(M,2)) = 0;
    V(1,J) = I;
return;
