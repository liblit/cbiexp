function clusterByMaxAspect()
    load runsinfo.mat
    load results.mat
    load oracle.mat
    Learn = configure();
    numbugaspects = Learn.Kb;
    Pz_d_Bug = Pz_d(end - numbugaspects + 1:end, :);
    [M,I] = max(Pz_d_Bug(:,Findices));
    for i = 1:numbugaspects;
        Clusters(i,Findices(find(I == i))) = 1;
    end;
    C = Clusters(:, Findices);
    c = size(C,1);
    B = Bugs(:, Findices);
    b = size(B,1);

    % pad matrices as necessary
    if b > c;
        C(size(C,1) + 1 : b, :) = 0; 
    elseif c > b;
        B(size(B,1) + 1 : c, :) = 0; 
    end;

    for i = 1:numbugaspects;
        Right = C(i,:) * B';
        Wrong = C(i,:) * not(B');
        Scores = Right - Wrong; 
        [m,index] = max(Scores);
        ClusterMap(i,1:3) = [index Right(index) Wrong(index)];  
    end;
    save clusters.mat Clusters ClusterMap
    quit
        

