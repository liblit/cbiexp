function runOnce()
    load('runsinfo.mat');
    Learn = configure();
    seed = sum(100*clock);
    rand('state',seed);

    [Pw_z,Pd_z,Pz,Pz_d, Li] = pLSA_EM(X,Learn,Sindices);

    save -mat results.mat Pw_z Pd_z Pz Pz_d Li seed

    out = fopen('loglikelihood.txt', 'w');
    fprintf(out, '%.0f\n', Li(end));
    fclose(out);

    quit;
