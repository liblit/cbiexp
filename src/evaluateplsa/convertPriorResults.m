function convertPriorResults(datafilename, infilename, outfilename)
    load(datafilename);
    Votes = load(infilename);

    % convert to one based indexing instead of zero based indexing
    Votes = [Votes(:,1) + 1 Votes(:,2) + 1];

    % swap row indices and column indices, all my stuff expects runs to be
    % columns
    Votes = Votes(:, [2 1]);

    % add a vector of ones on the right, so that spconvert will interpret
    % correctly
    Votes = [Votes ones(size(Votes,1),1)];

    % prepare to pass to conversion function 
    Votes = convertInfo(Votes, Data); 
    save('-mat', outfilename, 'Votes')
