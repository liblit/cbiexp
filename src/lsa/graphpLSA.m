% Change directory to the actual working directory where the data is 
cd '/scratch/mulhern/cbiexp_cvs/exif/analysis'

%load failure and success vectors
Findices = load('f.m');
Sindices = load('s.m');
Bugs = load('bug_runs.m');
Bugs = spconvert(Bugs);

%Do a series of runs for different K and write the results to a file

for k = 2:10
    inputfile = ['probabilities', int2str(k)] 
    eval(['load ', inputfile]);

    % graph feature | concept 
    imagesc(Pw_z); colormap(gray); colorbar;
    set(gca,'XTick',[1:k]);
    xlabel('Topic'); ylabel('Feature'); title('Pw|z density');
    outputfile = ['feature_probabilities', int2str(k)];
    print(gcf, '-djpeg', outputfile);

    %build legend
    for i = 1:k
        leg{i} = ['z = ', int2str(i)]; 
    end

    % graph concept | run
    colormap('default');
    [N,X] = hist(Pz_d');
    bar(repmat(X,1,size(N,2)),N);
    legend(leg, 'Location', 'BestOutside');
    title('Probability of aspect in all runs');
    xlabel('Probability increment');
    ylabel('Number of runs');
    outputfile = ['aspect_probabilities', int2str(k)];
    print(gcf, '-djpeg', outputfile);


    %graph concept | run for failures
    [N,X] = hist(Pz_d(:, Findices)');
    bar(repmat(X,1,size(N,2)),N);
    legend(leg, 'Location', 'BestOutside');
    title('Probability of aspect in failing runs');
    xlabel('Probability increment');
    ylabel('Number of runs');
    outputfile = ['aspect_probabilities_F', int2str(k)];
    print(gcf, '-djpeg', outputfile);

    %graph concept | run for successes
    [N,X] = hist(Pz_d(:, Sindices)');
    bar(repmat(X,1,size(N,2)),N);
    legend(leg, 'Location', 'BestOutside');
    title('Probability of aspect in succeeding runs');
    xlabel('Probability increment');
    ylabel('Number of runs');
    outputfile = ['aspect_probabilities_S', int2str(k)];
    print(gcf, '-djpeg', outputfile);

    %graph concept | run for bugs 
    for run_index = 1:size(Bugs,1)
        [N,X] = hist(Pz_d(:, find(Bugs(run_index,:)))');
        bar(repmat(X,1,size(N,2)),N);
        legend(leg, 'Location', 'BestOutside');
        title(['Probability of aspect in runs attributed to bug', int2str(run_index)]);
        xlabel('Probability increment');
        ylabel('Number of runs');
        outputfile = ['aspect_probabilities_', int2str(run_index), '_', int2str(k)];
        print(gcf, '-djpeg', outputfile);
    end
end

