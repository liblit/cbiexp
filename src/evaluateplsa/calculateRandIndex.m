function calculateRandIndex(bugfile, resultsfile, datafile, outputfile)
    load(bugfile);
    load(resultsfile); 
    load(datafile);
    Cluster = clusterByMaxValue(Results.Pz_d(Results.Learn.BuggyIndices,:));
    Bugs = Bugs(:,Data.Indices);

    Findices = find(Data.Findices); 
    Cluster = Cluster(:, Findices);
    Bugs = Bugs(:, Findices);

    if any(sum(Bugs,1) < 1);
        error('Failing run must have at least one bug cause.')
    end;

    numfailures = numel(Findices); 

    % each run gets a unique id; the indices of the bug causes in the array
    % determine a binary number
    Bugs = (Bugs' * (2.^((1:size(Bugs,1)) - 1))')';

    num = 0;
    for i = 1:numfailures;
        b = Bugs(i); 
        c = Cluster(i);
        for j = (i + 1):numfailures;
            b_other = Bugs(j); 
            c_other = Cluster(j);
            agreement = or(and(b == b_other, c == c_other), and(b ~= b_other, c ~= c_other));
            num = num + agreement;
        end
    end

    Rand = num/double(nchoosek(numfailures, 2));

    out = fopen(outputfile, 'w');
    fprintf(out, '%.4f\n', Rand);
    fclose(out);

    quit;
