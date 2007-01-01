function compareClustersToOracle()
    load runsinfo.mat
    load oracle.mat
    load clusters.mat
    Learn = configure();

    numbugaspects = Learn.Kb;

    C = spones(BugClusters(:,Findices));
    c = size(C,1);
    B = Bugs(:,Findices); 
    b = size(B,1);

    % pad matrices as necessary
    if b > c;
        C(c + 1 : b, :) = 0; 
    elseif c > b;
        B(b + 1 : c, :) = 0; 
    end;

    for i = 1:numbugaspects;
        Right = C(i,:) * B';
        Wrong = C(i,:) * not(B');
        Scores = Right - Wrong; 
        [m,index] = max(Scores);
        ClusterMap(i,1:3) = [index Right(index) Wrong(index)];  
    end;

    OracleByUsage = Bugs * spones(UsageClusters)';

    save clustermap.mat ClusterMap OracleByUsage;
    quit
        

