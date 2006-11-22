analysis_dir = '/scratch/mulhern/cbiexp_cvs/exif/analysis';
% Change directory to the actual working directory where the data is 
cd(analysis_dir); 

path(path, '/scratch/mulhern/cbiexp/src/lsa/plsa');

stat_dir = [analysis_dir '/stats'];
mkdir(stat_dir);

%Do a series of runs for different K and write the results to a file

for k = 2:9
    cur_dir = [stat_dir '/' int2str(k)];
    mkdir(cur_dir);

    % stats constrained runs
    if k > 2
        mode_dir = [cur_dir '/constrained']; 
        mkdir(mode_dir);
        inputfile = ['probabilities_C_', int2str(k)]; 
        eval(['load ', inputfile]);
        sortFeatures(Pw_z, mode_dir);
    end;

    %graph unconstrained runs
    mode_dir = [cur_dir '/unconstrained'];
    mkdir(mode_dir);
    inputfile = ['probabilities_' int2str(k)];
    eval(['load ', inputfile]); 
    sortFeatures(Pw_z, mode_dir);

end

