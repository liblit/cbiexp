function generateVotes(resultsfile, outputfile) 
    load(resultsfile);
    Votes = clusterByMaxValue(Results.Pz_d(Results.Learn.BuggyIndices, :));
    for i = 1:numel(Results.Learn.BuggyIndices);
        Votes(find(Votes == i)) = Results.Learn.BuggyIndices(i); 
    end;

    save('-mat', outputfile, 'Votes')
