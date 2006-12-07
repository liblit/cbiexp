analysis_dir = '/scratch/mulhern/cbiexp_cvs/exif/analysis';
% Change directory to the actual working directory where the data is 
cd(analysis_dir); 

path(path, '/scratch/mulhern/cbiexp/src/lsa/plsa');

%load failure and success vectors
Findices = load('f.indices');
Sindices = load('s.indices');
Bugs = load('bug_runs.sparse');
Bugs = spconvert(Bugs);

graph_dir = [analysis_dir '/graphs'];
mkdir(graph_dir);

%Do a series of runs for different K and write the results to a file

for k = 2:9
    cur_dir = [graph_dir '/' int2str(k)];
    mkdir(cur_dir);

    % graph constrained runs
    if k > 2
        mode_dir = [cur_dir '/constrained']; 
        mkdir(mode_dir);
        inputfile = ['probabilities_C_', int2str(k)]; 
        eval(['load ', inputfile]);

        probabilityDensity(Pw_z, mode_dir);

    end;

    %graph unconstrained runs
    mode_dir = [cur_dir '/unconstrained'];
    mkdir(mode_dir);
    inputfile = ['probabilities_' int2str(k)];
    eval(['load ', inputfile]); 
    probabilityDensity(Pw_z, mode_dir);

    if k == 2
        graphsFor2Aspects(Pw_z, Pz, Pd_z, Pz_d, Findices, Sindices, Bugs,mode_dir);
    end;


end

