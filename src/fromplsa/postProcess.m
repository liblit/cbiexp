function postProcess(inputfile, outputfile)
    load(inputfile);
    Results = deriveProbabilities(Results);
    Learn = Results.Learn;
    save('-mat', outputfile, 'Results')
