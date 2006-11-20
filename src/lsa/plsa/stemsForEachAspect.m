function stemsForEachAspect(Pz_d, Findices, Sindices, Bugs, K)

    numbugs = size(Bugs,1);

    if numbugs > 7
        error('I can not graph more than 7 bugs. My developer needs to figure out a more general way to handle colors.'); 
    end;
    colorvector = ['b'; 'g'; 'r'; 'c'; 'm'; 'y'; 'k'];
    symbolvector = ['o'; 'x'; '+'; '*'; 's'; 'd'; '^'];

    numsuccesses = numel(Sindices);
    % partition Pz_d, succeeding runs, runs corresponding to each bug
    % build index vectors for plots as we go
    S = Pz_d(:,Sindices);
    for bug = 1:numbugs
        B{bug} = Pz_d(:,find(Bugs(bug,:)));
        counts(bug) = size(B{bug},2);
    end;

    %computations until starts contains starting indices for each index vector
    counts(numbugs + 1) = 0;
    counts = circshift(counts, [0 1]);
    cumcounts = cumsum(counts); 
    starts = cumcounts + 1;

    %now generate index vectors for bugs themselves
    for i = 1:numel(starts) - 1
        Inds{i} = starts(i):starts(i+1) - 1;
    end;

    %make a stem plot for each aspect
    for i = 1:K

        %plot failures
        for bug = 1:numbugs
            Bs{bug} = sort(B{bug}(i,:), 'descend'); 
        end;
        for bug = 1:numbugs
            plot(Inds{bug}, Bs{bug}, [colorvector(bug) symbolvector(bug)]); 
            hold on;
        end;
        hold off;
        title(['Aspect ' int2str(i)]);
        ylabel('Probability this aspect is in this run'); 
        ylim([0 1]);
        xlim([0 numel(Findices)]);
        print(gcf, '-djpeg', ['aspect_probability_F_' int2str(i) '_' int2str(K)]);

        %plot successes
        Ss = sort(S(i,:),'descend'); 
        plot(Ss, 'k.');
        title(['Aspect ' int2str(i)]);
        ylabel('Probability this aspect is in this run'); 
        ylim([0 1]);
        xlim([0 numel(Sindices)]);
        print(gcf, '-djpeg', ['aspect_probability_S_' int2str(i) '_' int2str(K)]);
    end;

return;
