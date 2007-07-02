function runOnce(inputfile, outputfile, logfile, numaspects, numbugaspects, maxiterations, minlikelihoodchange)
    load(inputfile);
    Learn.Kb = numbugaspects;
    Learn.K = numaspects;
    Learn.Max_Iterations = maxiterations;
    Learn.Min_Likelihood_Change = minlikelihoodchange;

    if nnz(Data.X) == 0;
        warning('I can not compute results without data');
        quit;
    end;

    seed = sum(100*clock);
    Results.seed = seed;
    rand('state',seed);

    [Results.Pw_z,Results.Pd_z,Results.Pz,Results.Li] = pLSA_EM(Data.X, Learn, find(Data.Sindices));

    save('-mat', outputfile', 'Results'); 
    
    out = fopen(logfile, 'w');
    fprintf(out, '%.0f\n', Results.Li(end));
    fclose(out);

    quit;
