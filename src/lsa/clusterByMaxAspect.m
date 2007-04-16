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
    BugClusters = findBestIndex(Pz_d,Rows, adjustIndices(Findices,Indices));

    % for all runs, find usage aspects with maximum probability
    UsageClusters = findBestIndex(Pz_d, [1:numusageaspects], [1:numruns]);

    BugByUsage = spones(BugClusters) * spones(UsageClusters)'; 

    %for all runs, find aspect with maximum probability
    Clusters = findBestIndex(Pz_d, [1:numaspects], [1:numruns]);

    save clusters.mat BugClusters UsageClusters BugByUsage Clusters
    quit

% Res - m * n matrix, if the ith value is the maximum for the jth column, 
% Res(i,j) = X(i,j), else, Res(i,j) = 0.
% X - the original m * n matrix 
% Rows - a row selection vector
% Cols - a column selection vector
function Res = findBestIndex(X,Rows,Cols)
    P = X(Rows,Cols);  
    [M,I] = max(P,[],1);
    for i = 1:size(P,1);
        v = find(I == i); 
        Res(i,Cols(v)) = M(v);
    end;
    Res = padColumns(Res, size(X,2));  
    return;

function O = padColumns(O,n)
    if size(O,2) < n;
        O(size(O,1),n) = 0;
    end;
