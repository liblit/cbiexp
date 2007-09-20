function Clusters = clusterByMaxValue(X) 
    [M,Clusters] = max(X,[],1);
