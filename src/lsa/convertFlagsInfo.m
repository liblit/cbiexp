function convertFlagsInfo()
    F = load('flags.sparse'); 
    dimensions = load('X.dimensions');
    Flags = convertFlags(F, dimensions);
    save -mat flagsinfo.mat Flags
    quit; 

% Pads the matrix to the appropriate length
function X = convertFlags(X,dimensions)
    X = spconvert(X);
    if not(dimensions(2) == size(X,2));
        X(size(X,1),dimension(2)) = 0;
    end;
