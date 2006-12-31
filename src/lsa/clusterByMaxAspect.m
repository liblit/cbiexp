function clusterByMaxAspect()
    load runsinfo.mat
    load results.mat
    Learn = configure();
    numaspects = Learn.K;

    if numaspects ~= size(Pz_d,1);
       error('number of aspects and size of probability matrix disagree');
    end;

    numruns = size(Pz_d,2);

    numbugaspects = Learn.Kb;
    numusageaspects = numaspects - numbugaspects; 

    % for failures, find buggy aspect with maximum probability 
    Rows = numusageaspects + 1:numaspects;
    BugClusters = findBestAspect(Pz_d,Rows, Findices);
    BugClusters = padColumns(BugClusters, numruns);

    % for all runs, find usage aspects with maximum probability
    UsageClusters = findBestAspect(Pz_d, [1:numusageaspects], [1:numruns]);

    BugByUsage = spones(BugClusters) * spones(UsageClusters)'; 

    %for all runs, find aspect with maximum probability
    Clusters = findBestAspect(Pz_d, [1:numaspects], [1:numruns]);

    save clusters.mat BugClusters UsageClusters BugByUsage Clusters
    quit

function Res = findBestAspect(X,Rows,Cols)
    P = X(Rows,:);  
    [M,I] = max(P(:,Cols));
    for i = 1:size(P,1);
        v = find(I == i); 
        Res(i,Cols(v)) = M(v);
    end;

function O = padColumns(O,n)
    if size(O,2) < n;
        O(size(O,1),n) = 0;
    end;
