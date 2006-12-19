function convertData()
    Findices = load('f.indices');
    Sindices = load('s.indices');
    X = load('X.sparse');
    dimensions = load('X.dimensions');
    if numel(X) == 0;
        X = sparse(dimensions(1), dimensions(2));
        warning('X.sparse has only zero entries.')
    else
        X = spconvert(X);
        if not(all(dimensions == size(X)));
            X(dimensions(1), dimensions(2)) = 0;
        end;
    end;
    save -mat runsinfo.mat Findices Sindices X
    quit; 

