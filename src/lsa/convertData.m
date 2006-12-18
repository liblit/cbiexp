function convertData()
    Findices = load('f.indices');
    Sindices = load('s.indices');
    X = load('X.sparse');
    if numel(X) == 0;
        X = sparse(0,0);
        warning('X.sparse has zero entries.')
    else
        X = spconvert(X);
    end;
    save -mat runsinfo.mat Findices Sindices X
    quit; 

