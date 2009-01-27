% function convertData(inputfile, outputfile, ffile, sfile, ifile, dimfile)
%
% converts data from a textual representation to a straightforward matlab
% representation. 
%
% The X matrix is padded to the correct size in case some counts at the edges
% are zero.
% Fvector and Svector are stored as bit vectors.
% Indices is actually a vector of 1 based indices.
%
% Fails gracelessly if any files are specified incorrectly. 
%
% inputfile -- file that contains counts in sparse matrix format
% outputfle -- .mat outputfile
% outcomes -- file specifying outcomes  
% causes -- file specifying causes 
% dimfile -- the miscellaneous info about the experiment 

function convertData(inputfile, outputfile, outcomes, causes, dimfile)

    % get miscellaneous info about experiment
    dims = xmlread(dimfile);
    data = dims.getDocumentElement();
    numpreds = str2num(data.getAttribute('numpreds'));
    numruns = str2num(data.getAttribute('numruns'));

    % load data
    Outcomes = int8(load(outcomes));
    Fvector = Outcomes > 0;
    Svector = Outcomes == 0;
    Indices = find(sum([Fvector Svector], 2));
    Causes = int8(load(causes)); 
    X = load(inputfile);
    X = convertCounts(X, [numpreds numruns]);

    % save data
    Data.X = X;
    Data.Fvector = Fvector;
    Data.Svector = Svector;
    Data.Indices = Indices;
    Data.Causes = Causes;
    Data.inputfile = inputfile;
    save('-mat', outputfile, 'Data'); 

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
