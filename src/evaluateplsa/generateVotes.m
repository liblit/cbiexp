function generateVotes(resultsfile, outputfile) 
    load(resultsfile);
    Votes = clusterByMaxValue(Results.Pz_d(Results.Learn.BuggyIndices, :));

    save('-mat', outputfile, 'Votes')
