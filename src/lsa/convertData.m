function convertData()
    Findices = load('f.indices');
    Sindices = load('s.indices');
    Z = load('X.sparse');
    for i = 1:size(Z,1);
        X(Z(i,1),Z(i,2)) = 1;
    end;
    save -mat runsinfo.mat Findices Sindices X
    quit; 

