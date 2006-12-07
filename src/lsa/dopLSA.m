% Put pLSA scripts in path
path (path, '/scratch/mulhern/cbiexp/src/lsa/plsa')

% Change directory to the actual working directory where the data is 
cd '/scratch/mulhern/sliver/grep/builds/v4/FAULTY_F_DG_3+FAULTY_F_KP_6+FAULTY_F_KP_8/analysis'

%Load values for predicate * run matrix 
Xsparse = load('X.sparse');
X = spconvert(Xsparse);
clear Xsparse;

%normalize X
C = sum(X,1); 
X = X * diag(spfun(inline('1./x'), C)); 

%Load indices of succesful runs
Sindices = load('s.indices');

density = nnz(X)/prod(size(X))

%Do a series of runs for different K and write the results to a file

%First set the parameters for the executions
%I don't know much about them, so we can accept the example values 
Learn.Verbosity = 1;
Learn.Max_Iterations = 400; 
Learn.heldout = .1; % for tempered EM only, percentage of held out data
Learn.Min_Likelihood_Change = 1;
Learn.Folding_Iterations = 20; 
Learn.TEM = 0; %not tempered

k = 9;

for r = 1:12

    start = clock;
    Learn.Constrained = 1;
    Learn.Kb = 3;
    [Pw_z,Pd_z,Pz,Pz_d, Li] = pLSA_EM(X,k,Learn,Sindices);
    time = etime(clock, start)
    outputfile = ['probabilities_C_', int2str(r)]; 
    eval(['save ', outputfile, ' Pw_z Pd_z Pz Pz_d Li time']) 

    start = clock;
    Learn.Constrained = 0;
    [Pw_z,Pd_z,Pz,Pz_d, Li] = pLSA_EM(X,k,Learn);
    time = etime(clock, start);
    outputfile = ['probabilities_U_', int2str(r)];
    eval(['save ', outputfile, ' Pw_z Pd_z Pz Pz_d Li time']) 

end
