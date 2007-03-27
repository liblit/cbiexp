function compareClustersToOracle()
    load runsinfo.mat
    load oracle.mat
    load clusters.mat
    Learn = configure();

    numbugaspects = Learn.Kb;

    C = spones(BugClusters(:,Findices));
    B = Bugs(:,Findices); 

    Right = C * B';
    Wrong = C * not(B');
    Scores = Right - Wrong;
    ClusterMat = zeros(size(Scores));
    Is = ones(size(ClusterMat, 1), 1); 
    Js = ones(1, size(ClusterMat, 2));   
    while (any(Is) & any(Js));
        Iinds = find(Is);
        Jinds = find(Js);
        [M,I] = max(Scores(Iinds, Jinds), [], 1);
        [m,j] = max(M);
        ClusterMat(Iinds(I(j)), Jinds(j)) = 1;
        Is(Iinds(I(j)),1) = 0;
        Js(1, Jinds(j)) = 0;
    end;

    save clustermap.mat ClusterMat Scores Right Wrong
    quit
        

