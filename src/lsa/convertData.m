function convertData()
    Findices = load('f.indices');
    Sindices = load('s.indices');
    dimensions = load('X.dimensions');
    X = load('Xobs.sparse');
    Xobs = convertCounts(X,dimensions);
    X = load('Xtru.sparse');
    Xtru = convertCounts(X,dimensions);
    save -mat runsinfo.mat Findices Sindices Xobs Xtru
    quit; 

function X = convertCounts(X,dimensions)
    if numel(X) == 0;
        X = sparse(dimensions(1), dimensions(2));
        warning('counts matrix  has only zero entries.')
    else
        X = spconvert(X);
        if not(all(dimensions == size(X)));
            X(dimensions(1), dimensions(2)) = 0;
        end;
    end;
    
