function convertOracle()
    Bugs = load('oracle.sparse');
    dimensions = load('X.dimensions');
    Bugs = spconvert(Bugs);
    if not(dimensions(2) == size(Bugs,2));
        Bugs(size(Bugs,1),dimensions(2)) = 0;
    end;
    save -mat oracle.mat Bugs 
    quit; 

