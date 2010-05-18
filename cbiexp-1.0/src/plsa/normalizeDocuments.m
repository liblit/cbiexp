function normalizeDocuments(inputfile, outputfile)
    load(inputfile);
    Data.X = normalizeColumns(Data.X);
    save('-mat', outputfile, 'Data');
