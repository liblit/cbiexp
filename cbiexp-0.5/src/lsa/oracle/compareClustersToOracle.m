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
    ClusterMat = makeMap(Scores);
    Confusion = makeConfusion(ClusterMat, C, B); 
    save clustermap.mat ClusterMat Scores Right Wrong Confusion
    quit

function Map = makeMap(Scores)
    Map = zeros(size(Scores));
    Js = ones(1, size(Map, 2));   
    Is = ones(size(Map,1), 1);
    while (any(Js) & any(Is));
        Iinds = find(Is);
        Jinds = find(Js);
        [M,I] = max(Scores(Iinds, Jinds), [], 1);
        [m,j] = max(M);
        Map(Iinds(I(j)), Jinds(j)) = 1;
        Is(Iinds(I(j)), 1) = 0;
        Js(1, Jinds(j)) = 0;
    end;

function Confusion = makeConfusion(ClusterMat, C, B)
    for i = 1:size(ClusterMat,1);
        v = find(ClusterMat(i,:)); 
        C1(v,:) = repmat(C(i,:), numel(v), 1); 
    end;
    Confusion = (C1 * B')';
