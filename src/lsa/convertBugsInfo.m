function convertBugsInfo(outfilename)
    Bugs = load('bugs.sparse'); 
    Xdimensions = load('X.dimensions');
    Indexing = indexing();
    Bugs = convertInfo(Bugs, Xdimensions(2), Indexing);
    save('-mat', outfilename, 'Bugs')
