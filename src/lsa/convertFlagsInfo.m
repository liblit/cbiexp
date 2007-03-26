function convertFlagsInfo()
    F = load('flags.sparse'); 
    Xdimensions = load('X.dimensions');
    Fdimensions = load('flags.dimensions');
    Flags = convertFlags(F, [Fdimensions(1) Xdimensions(2)]);
    save -mat flagsinfo.mat Flags
    quit; 

% Pads the matrix to the appropriate length
function X = convertFlags(X,dimensions)
    X = spconvert(X);
    if not(all(dimensions == size(X)));
        X(dimensions(1),dimensionis(2)) = 0;
    end;
