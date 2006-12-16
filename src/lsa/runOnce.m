function runOnce()
    load('runsinfo.mat');
    Learn = configure();
    seed = sum(100*clock);
    rand('state',seed);

    [Pw_z,Pd_z,Pz,Pz_d, Li] = pLSA_EM(pLSAwrapper(X,Findices,Sindices), Learn,Sindices);

    save -mat results.mat Pw_z Pd_z Pz Pz_d Li seed

    out = fopen('loglikelihood.txt', 'w');
    fprintf(out, '%.0f\n', Li(end));
    fclose(out);

    quit;

function N = pLSAwrapper(X,Findices, Sindices)
    N = X;
    if numel(Findices) + numel(Sindices) < size(X,2)
        R = zeros(size(X,2),1);
        R(Findices) = ones(numel(Findices),1);
        R(Sindices) = ones(numel(Sindices),1);
        Z = find(R == 0); 
        N(:,Z) = zeros(size(X,1), numel(Z));
    end;
    
