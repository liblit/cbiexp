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

    [Pw_z,Pd_z,Pz,Pz_d, Li] = pLSA_EM(X, Learn, Sindices);

    save -mat results.mat Pw_z Pd_z Pz Pz_d Li seed Learn

    out = fopen('loglikelihood.txt', 'w');
    fprintf(out, '%.0f\n', Li(end));
    fclose(out);

    quit;
