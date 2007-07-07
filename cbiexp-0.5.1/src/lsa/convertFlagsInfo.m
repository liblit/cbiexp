function convertFlagsInfo(outfilename)
    F = load('flags.sparse'); 
    Xdimensions = load('X.dimensions');
    Indexing = indexing();
    Flags = convertInfo(F, Xdimensions(2), Indexing); 
    save('-mat', outfilename, 'Flags')
