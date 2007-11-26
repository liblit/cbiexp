% function convertData(inputfile, outputfile, ffile, sfile, ifile, dimfile)
%
% converts data from a textual representation to a straightforward matlab
% representation. 
%
% The X matrix is padded to the correct size in case some counts at the edges
% are zero.
% Findices and Sindices are stored as padded bit vectors.
% Indices is actually a vector of indices. If Indices(i) = x, then the ith
% column in X corresponds to the offset + x - 1 run in a CBI data directory.
% The offset keeps track of the training runs---the 1 is subtracted as we go
% from 1 to 0 based indexing.
%
% All discarded runs, i.e., runs that are neither failures or successes are
% forced to zero. 
%
% Fails gracelessly if any files are specified incorrectly. 
%
% inputfile -- file that contains counts in sparse matrix format
% outputfle -- .mat outputfile
% ffile -- contains indices of failing runs
% sfile -- contains indices of succeeding runs
% ifile -- contains indices of discarded runs
% dimfile -- the miscellaneous info about the experiment 

function convertData(inputfile, outputfile, ffile, sfile, ifile, dimfile)

    % get miscellaneous info about experiment
    dims = xmlread(dimfile);
    data = dims.getDocumentElement();
    numpreds = str2num(data.getAttribute('numpreds'));
    numruns = str2num(data.getAttribute('numruns'));
    offset = str2num(data.getAttribute('runoffset'));

    % load data
    Findices = load(ffile);
    Sindices = load(sfile);
    Dindices = load(ifile);
    Findices = explode(Findices, numruns);
    Sindices = explode(Sindices, numruns);
    Dindices = explode(Dindices, numruns);
    verifyWellFormed([Findices Sindices Dindices], numruns);
    Indices = find(sum([Findices Sindices], 2));
    X = load(inputfile);
    X = convertCounts(X, [numpreds numruns]);

    % discard discards
    X = X(:, Indices);
    Findices = Findices(Indices,:);
    Sindices = Sindices(Indices, :);
    if any((Dindices(Indices, :)) ~= 0)
        error('discards not correctly discarded');
    end

    % save data
    Data.X = X;
    Data.Findices = Findices;
    Data.Sindices = Sindices;
    Data.Indices = Indices;
    Data.offset = offset;
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
