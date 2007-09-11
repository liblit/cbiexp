function makeProbabilityFile(resultsfile, aspectnum, outputfile)
    load(resultsfile);
    Pz_w = Results.Pz_w(aspectnum,:);
    dlmwrite(outputfile, Pz_w, '\n');
