function runOnce()
    load('runsinfo.mat');
    Learn = configure();
    if Learn.TruthCounts;
        X = Xtru;
    else;
        X = Xobs;
    end;

    if numel(X) == 0;
        warning('I can not compute results without data');
        quit;
    end;

    seed = sum(100*clock);
    rand('state',seed);

    [Pw_z,Pd_z,Pz,Li] = pLSA_EM(X, Learn, Sindices);
    
    Pw = marginalize(Pw_z, Pz);
    Pd = marginalize(Pd_z, Pz);
    Pz_d = applyBayes(Pd_z,Pz,Pd);
    Pz_w = applyBayes(Pw_z,Pz,Pw); 
    Pd_w = Pd_z * Pz_w;
    Pw_d = Pw_z * Pz_d; 
    [Sd, Id] = sortMinDiffs(Pz_d);
    [Sw, Iw] = sortMinDiffs(Pz_w);

    save -mat results.mat Pw_z Pd_z Pz Pw Pd Pz_w Pz_d Pd_w Pw_d Sd Id Sw Iw Li seed Learn

    out = fopen('loglikelihood.txt', 'w');
    fprintf(out, '%.0f\n', Li(end));
    fclose(out);

    quit;
