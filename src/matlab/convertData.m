% function convertData(inputfile, outputfile, ffile, sfile, ifile, dimfile)
%
% converts data from a textual representation to a straightforward matlab
% representation. 
%
% The X matrix is padded to the correct size in case some counts at the edges
% are zero.
% Findices and Sindices are stored as padded bit vectors.
% Indices is actually a vector of indices 1 based indices.
%
% Fails gracelessly if any files are specified incorrectly. 
%
% inputfile -- file that contains counts in sparse matrix format
% outputfle -- .mat outputfile
% ffile -- contains indices of failing runs
% sfile -- contains indices of succeeding runs
% dimfile -- the miscellaneous info about the experiment 

function convertData(inputfile, outputfile, ffile, sfile, dimfile)

    % get miscellaneous info about experiment
    dims = xmlread(dimfile);
    data = dims.getDocumentElement();
    numpreds = str2num(data.getAttribute('numpreds'));
    numruns = str2num(data.getAttribute('numruns'));

    % load data
    Findices = load(ffile);
    Sindices = load(sfile);
    Findices = explode(Findices, numruns);
    Sindices = explode(Sindices, numruns);
    verifyWellFormed([Findices Sindices], numruns);
    Indices = find(sum([Findices Sindices], 2));
    X = load(inputfile);
    X = convertCounts(X, [numpreds numruns]);

    % save data
    Data.X = X;
    Data.Findices = Findices;
    Data.Sindices = Sindices;
    Data.Indices = Indices;
    Data.inputfile = inputfile;
    save('-mat', outputfile, 'Data'); 

% Checks that every run has a designation and that no runs have two
%
function verifyWellFormed(X, l)
    if size(X,1) ~= l
        error('classify vectors are wrong size');
    end

    if any(sum(X,2) ~= 1) 
        error('some runs are dual classified or unclassified');
    end

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
