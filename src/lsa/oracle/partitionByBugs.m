function partitionByBugs()
    load('results.mat');
    load('runsinfo.mat');
    load('oracle.mat');

    Fvector(size(X,2), 1) = 0; 
    Fvector(Findices) = 1;
    for i = 1:size(Bugs, 1);
        Pz_d_Bugs{i} = Pz_d(:, find(and(Bugs(i,:),Fvector')));
    end;
    save -mat Pz_d_Bugs.mat Pz_d_Bugs
    quit;
