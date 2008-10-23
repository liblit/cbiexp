% function D = removeAllZeroRuns(Data)
%
% removes runs where the sum of all feature counts is zero
% updates X, Indices, Fvector, Svector 
% assumes that Data contains X, Indices, Fvector, Svector

function D = removeAllZeroRuns(Data)
    D = Data;
    NonZeros = find(sum(Data.X,1));
    D.Fvector = Data.Fvector(NonZeros',:); 
    D.Svector = Data.Svector(NonZeros',:); 
    D.Causes = Data.Causes(NonZeros',:);
    D.X = Data.X(:, NonZeros);
    D.Indices = Data.Indices(NonZeros',:);
