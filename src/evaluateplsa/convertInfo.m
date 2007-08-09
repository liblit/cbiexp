% Pads the matrix to the appropriate length and slices all runs that were not
% part of test
function X = convertInfo(X, Data, ignoreoffset)
    X = spconvert(X);
    if ~ignoreoffset;
        X = X(:,Data.offset + 1 : end);
    end
    maxindex = Data.Indices(end);
    if size(X,2) < maxindex; 
        X(end, maxindex) = 0; 
    end;
    X = X(:, Data.Indices);
