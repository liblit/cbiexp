function convertBugsInfo(outfilename)
    Bugs = load('bugs.sparse'); 
    Xdimensions = load('X.dimensions');
    Indexing = indexing();
    Bugs = convertBugs(Bugs, Xdimensions(2), Indexing);
    save('-mat', outfilename, 'Bugs')

% Pads the matrix to the appropriate length
function X = convertBugs(X, numruns, Indexing)
    X = spconvert(X);
    X = X(:,Indexing.offset + 1 : end);
    if not(all(numruns == size(X,2)));
        X(end, numruns) = 0;
    end;
