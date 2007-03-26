function convertBugsInfo()
    Bugs = load('bugs.sparse'); 
    Xdimensions = load('X.dimensions');
    Bdimensions = load('bugs.dimensions');
    Bugs = convertBugs(Bugs, [Bdimensions(1) Xdimensions(2)]);
    save -mat bugsinfo.mat Bugs
    quit; 

% Pads the matrix to the appropriate length
function X = convertBugs(X,dimensions)
    X = spconvert(X);
    if not(all(dimensions == size(X)));
        X(dimensions(1),dimensions(2)) = 0;
    end;
