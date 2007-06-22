function convertData(outputfile)
    Findices = load('f.indices');
    Sindices = load('s.indices');
    dimensions = load('X.dimensions');
    X = load('Xobs.sparse');
    Xobs = buildCounts(X,dimensions, Findices, Sindices);
    X = load('Xtru.sparse');
    Xtru = buildCounts(X,dimensions, Findices, Sindices);
    save('-mat', outputfile, 'Findices', 'Sindices', 'Xobs', 'Xtru')

% Build X
function X = buildCounts(X, dimensions, Findices, Sindices)
  X = zeroDiscards(convertCounts(X, dimensions), Findices, Sindices);

% Pads the matrix to the appropriate length
function X = convertCounts(X,dimensions)
    if numel(X) == 0;
        X = sparse(dimensions(1), dimensions(2));
        warning('counts matrix  has only zero entries.')
    else
        X = spconvert(X);
        if not(all(dimensions == size(X)));
            X(dimensions(1), dimensions(2)) = 0;
        end;
    end;
    
% Makes sure that all discarded runs are actually all zero 
function X = zeroDiscards(X,Findices, Sindices)
    if numel(Findices) + numel(Sindices) < size(X,2)
        R = zeros(size(X,2),1);
        R(Findices) = ones(numel(Findices),1);
        R(Sindices) = ones(numel(Sindices),1);
        Z = find(R == 0); 
        X(:,Z) = zeros(size(X,1), numel(Z));
    end;
