function convertData()
    Findices = load('f.indices');
    Sindices = load('s.indices');
    X = load('X.sparse');
    X = spconvert(X);
    save -mat runsinfo.mat Findices Sindices X
    quit; 

