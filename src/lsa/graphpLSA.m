% Change directory to the actual working directory where the data is 
cd '/scratch/mulhern/cbiexp_cvs/exif/analysis'

path(path, '/scratch/mulhern/cbiexp/src/lsa/plsa');

%load failure and success vectors
Findices = load('f.m');
Sindices = load('s.m');
Bugs = load('bug_runs.m');
Bugs = spconvert(Bugs);

%Do a series of runs for different K and write the results to a file

for k = 2:3
    inputfile = ['probabilities', int2str(k)] 
    eval(['load ', inputfile]);

    % graph feature | concept 
    imagesc(Pw_z); colormap(gray); colorbar;
    set(gca,'XTick',[1:k]);
    xlabel('Topic'); ylabel('Feature'); title('Pw|z density');
    outputfile = ['feature_probabilities', int2str(k)];
    print(gcf, '-djpeg', outputfile);

    % for each concept, graph probabilities for each run  
    stemsForEachAspect(Pz_d, Findices, Sindices, Bugs,k);

    if k == 2
        graphsFor2Aspects(Pw_z, Pz, Pd_z, Pz_d, Findices, Sindices, Bugs);
    end;

end

