% Turns an index vector into a bit vector of the correct length
% X -- the index vector
% l -- the length to explode to
function E = explode(X, l) 
    E = zeros(l, 1);
    E(X) = 1;
