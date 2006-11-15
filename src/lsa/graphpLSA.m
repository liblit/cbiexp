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

    % graph concept | run
    imagesc(Pz_d'); colormap(gray); colorbar;
    set(gca, 'XTick', [1:k]);
    xlabel('Topic'); ylabel('Run'); title('Pz|d density');
    outputfile = ['aspect_probabilities', int2str(k)];
    print(gcf, '-djpeg', outputfile);

    %graph concept | failing run
    imagesc(Pz_d(:, Findices)'); colormap(gray); colorbar;
    set(gca, 'XTick', [1:k]);
    xlabel('Topic'); ylabel('Failing Run'); title('Pz|d density');
    outputfile = ['aspect_probabilitiesF', int2str(k)];
    print(gcf, '-djpeg', outputfile);

    %graph concept | succeeding runs
    imagesc(Pz_d(:, Sindices)'); colormap(gray); colorbar;
    set(gca, 'XTick', [1:k]);
    xlabel('Topic'); ylabel('Succeeding Run'); title('Pz|d density');
    outputfile = ['aspect_probabilitiesS', int2str(k)];
    print(gcf, '-djpeg', outputfile);

    for run_index = 1:size(Bugs,1)

        imagesc(Pz_d(:, find(Bugs(run_index,:)))'); colormap(gray); colorbar;
        set(gca, 'XTick', [1:k]);
        xlabel('Topic'); ylabel(['Bug Run', int2str(run_index)]); title('Pz|d density');
        outputfile = ['aspect_probabilities', int2str(run_index), '_', int2str(k)];
        print(gcf, '-djpeg', outputfile);
        
    end
end

