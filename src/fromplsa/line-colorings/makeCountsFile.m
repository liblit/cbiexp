function makeCountsFile(datafile, votesfile, outputfile, index)
    load(datafile) 
    load(votesfile)

    slice = find(Votes == index);
    runs = Data.X(:, slice);
    avecounts = sum(runs, 2)./numel(slice);
    save('-ascii', outputfile, 'avecounts')
