function postProcess(inputfile, outputfile)
    load(inputfile);
    Results = deriveProbabilities(Results);
    Learn = Results.Learn;
    Results.buggyclusters = clusterByMaxValue(Results.Pz_d(Results.Learn.BuggyIndices, :));
    save('-mat', outputfile, 'Results')
