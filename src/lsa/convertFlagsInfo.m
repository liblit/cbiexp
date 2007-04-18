function convertFlagsInfo(outfilename)
    F = load('flags.sparse'); 
    Xdimensions = load('X.dimensions');
    Indexing = indexing();
    Flags = convertFlags(F, Xdimensions(2), Indexing); 
    save('-mat', outfilename, 'Flags')

% Pads the matrix to the appropriate length
function X = convertFlags(X, numruns, Indexing)
    X = spconvert(X);
    X = X(:, Indexing.offset + 1 : end);
    if not(all(numruns == size(X,2)));
        X(end, numruns) = 0;
    end;
