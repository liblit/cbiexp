% removes runs from existing data
% prefilename -- contains the data to be processed
% postfilename -- processed data is output here
% This function also removes runs with a feature count that sums to zero 
% This run selects the appropriate X matrix according to the value of
% Learn.TruthCounts
% It should output to postfilename a single X matrix, the approriate Findices
% and Sindices vectors for that reduced matrix, and an Indices vector
% consisting of the indices into the original matrix to which this squeezed
% matrix corresponds.
% If the fraction of succeeding runs remaining after zero runs are removed is
% less than the fraction specified by Learn.Fraction_Succeeding_Runs_Retained,
% no further runs are removed. 
function removeRuns(prefilename, postfilename)
    load (prefilename);
    Learn = configure();
    X = selectX(Xtru, Xobs, Learn);
    sfraction = Learn.Fraction_Succeeding_Runs_Retained;

    % first we remove runs that have all zero counts
    [NZX,NZI] = removeZeroColumns(X);
    NZSindices = adjustIndices(Sindices,NZI);
    NZFindices = adjustIndices(Findices,NZI);

    % then we calculate the number of additional succeeding runs to remove
    excess = numel(NZSindices) - round(sfraction * numel(Sindices));

    % we index randomly into a copy of the NZSindices vector 
    % removing one index at a time 
    V = NZSindices;
    rand('state', sum(100*clock));
    Zeros = zeros(size(NZX,1), 1);
    for i = 1:excess;
        V = V(randperm(numel(V)),1);
        NZX(:, V(1,1)) = Zeros;
        V = V(2:end,1); 
    end;

    % we remove the additional non-zero columns we've just created 
    [RZX, RZI] = removeZeroColumns(NZX);

    % we calculate the new version of vectors 
    Indices = NZI(RZI); 
    Sindices = adjustIndices(Sindices, Indices);
    Findices = adjustIndices(Findices, Indices);
    X = RZX;

    % and save them
    save('-mat', postfilename, 'X', 'Sindices', 'Findices', 'Indices');
