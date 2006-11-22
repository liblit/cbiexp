% Put pLSA scripts in path
path (path, '/scratch/mulhern/cbiexp/src/lsa/plsa')

% Change directory to the actual working directory where the data is 
cd '/scratch/mulhern/cbiexp_cvs/exif/analysis'

%Load values for predicate * run matrix 
Xsparse = load('X.dat');
X = spconvert(Xsparse);
clear Xsparse;

%Load indices of succesful runs
Sindices = load('s.m');

density = nnz(X)/prod(size(X))

%Do a series of runs for different K and write the results to a file

%First set the parameters for the executions
%I don't know much about them, so we can accept the example values 
Learn.Verbosity = 1;
Learn.Max_Iterations = 200; 
Learn.heldout = .1; % for tempered EM only, percentage of held out data
Learn.Min_Likelihood_Change = 1;
Learn.Folding_Iterations = 20; 
Learn.TEM = 0; %not tempered

for k = 2:9

    if k > 2
        Learn.Constrained = 1;
        Learn.Kb = 2;
        outputfile = ['probabilities_C_', int2str(k)]; 
        [Pw_z,Pd_z,Pz,Pz_d, Li] = pLSA_EM(X,k,Learn,Sindices);
        eval(['save ', outputfile, ' Pw_z Pd_z Pz Pz_d Li']) 
    end;

    Learn.Constrained = 0;
    outputfile = ['probabilities_', int2str(k)];
    [Pw_z,Pd_z,Pz,Pz_d, Li] = pLSA_EM(X,k,Learn);
    eval(['save ', outputfile, ' Pw_z Pd_z Pz Pz_d Li']) 

end
