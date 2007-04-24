% Pads the matrix to the appropriate length
function X = convertInfo(X, numruns, Indexing)
    X = spconvert(X);
    X = X(:,Indexing.offset + 1 : end);
    if not(all(numruns == size(X,2)));
        X(end, numruns) = 0;
    end;
