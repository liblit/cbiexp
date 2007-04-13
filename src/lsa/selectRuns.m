function selectRuns(prefilename, infofilename, postfilename)
    load (prefilename);
    load (infofilename); 

    Bugs = Bugs(:, Indices);

    save('-mat', postfilename, 'Bugs');
