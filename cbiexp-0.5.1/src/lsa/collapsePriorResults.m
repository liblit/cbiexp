function collapsePriorResults(infilename, outfilename)
    load(infilename);
    load runsinfo.mat;

    Votes = Votes(:,Indices);

    [I,J] = find(Votes);

    Runs_Preds = [J I]; 

    save('-mat', outfilename, 'Votes', 'Runs_Preds')
