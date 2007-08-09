function = calculateRandIndex(bugfile, resultsfile, datafile, outputfile)
    load(bugfile);
    load(resultsfile); 
    load(datafile);
    Cluster = clusterByMaxValue(Results.Pz_d(Results.Learn.BuggyIndices,:));
    Bugs = Bugs(:,Data.Indices);

    Cluster = Cluster(:,find(Data.Findices));
    Bugs = Bugs(:,find(Data.Findices));

    if any(sum(Bugs,1) < 1);
        error('Failing run must have at least one bug cause.')
    end;

    ms = find(sum(Bugs,1) == 1);

    Cluster = Cluster(:,ms);
    Bugs = Bugs(:,ms);

    num = 0;
    antinum = 0;
    for i = 1:size(Bugs,2);
        b = find(Bugs(:,i));
        c = Cluster(i);
        for j = i + 1:size(Bugs,2);
            b_other = find(Bugs(:,j));
            c_other = Cluster(j);
            agreement = or(and(b == b_other, c == c_other), and(b ~= b_other, c ~= c_other));
            num = num + agreement;
            antinum = antinum + ~agreement;
        end
    end

    Rand = num/double(nchoosek(numel(ms), 2));

    out = fopen(outputfile, 'w');
    fprintf(out, '%.4f\n', Rand);
    fclose(out);

    quit;
