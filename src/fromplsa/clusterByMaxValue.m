function Clusters = clusterByMaxValues(X) 
    [M,Clusters] = max(X,[],1);
