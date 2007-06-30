% function D = removeAllZeroRuns(Data)
%
% removes runs where the sum of all feature counts is zero
% updates X, Indices, Findices, Sindices 
% assumes that Data contains X, Indices, Findices, Sindices

function D = removeAllZeroRuns(Data)
    D = Data;
    NonZeros = find(sum(Data.X,1));
    D.Findices = Data.Findices(NonZeros',:); 
    D.Sindices = Data.Sindices(NonZeros',:); 
    D.X = Data.X(:, NonZeros);
    D.Indices = Data.Indices(NonZeros',:);
