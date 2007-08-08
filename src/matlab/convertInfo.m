% Pads the matrix to the appropriate length and slices all runs that were not
% part of test
function X = convertInfo(X, Data)
    X = spconvert(X);
    X = X(:,Data.offset + 1 : end);
    maxindex = Data.Indices(end);
    if size(X,2) < maxindex; 
        X(end, maxindex) = 0; 
    end;
    X = X(:, Indices);
