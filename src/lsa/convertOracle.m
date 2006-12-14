function convertOracle()
    Bugs = load('oracle.sparse');
    Bugs = spconvert(Bugs);
    save -mat oracle.mat Bugs 
    quit; 

