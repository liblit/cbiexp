function calculateRandIndex(votesfile, datafile, outputfile)
    load(votesfile); 
    load(datafile);
    Causes = Data.Causes;

    Findices = find(Data.Fvector); 
    Votes = Votes(:, Findices);
    Causes = Causes(Findices);

    numfailures = numel(Findices); 

    num = 0;
    for i = 1:numfailures;
        b = Causes(i); 
        c = Votes(i);
        for j = (i + 1):numfailures;
            b_other = Causes(j); 
            c_other = Votes(j);
            agreement = or(and(b == b_other, c == c_other), and(b ~= b_other, c ~= c_other));
            num = num + agreement;
        end
    end

    Rand = num/double(nchoosek(numfailures, 2));

    out = fopen(outputfile, 'w');
    fprintf(out, '%.4f\n', Rand);
    fclose(out);
