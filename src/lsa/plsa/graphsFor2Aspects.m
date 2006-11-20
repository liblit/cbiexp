function graphsFor2Aspects(Pw_z, Pz, Pd_z, Pz_d, Findices, Sindices, Bugs)

    doOneGraph(Pz_d(1,:), 'Probability of aspect 1 in all runs', 'aspect_probabilities_A_2');

    doOneGraph(Pz_d(1,Findices), 'Probability of aspect 1 in all failing runs', 'aspect_probabilities_F_2');

    doOneGraph(Pz_d(1,Sindices), 'Probability of aspect 1 in all succeeding runs', 'aspect_probabilities_S_2');

    %graph concept | run for bugs 
    for run_index = 1:size(Bugs,1)
        doOneGraph(Pz_d(1, find(Bugs(run_index,:))), ['Probability of aspect 1 in runs attributed to bug ' int2str(run_index)], ['aspect_probabilities_' int2str(run_index) '_2']);
    end

return;

function doOneGraph(V, t, outputfile)   
    colormap('default');
    X = 0.5:0.1:0.95;
    N = hist(V,X);
    bar(X,N,1);
    title(t);
    xlabel('Probability increment');
    ylabel('Number of runs');
    print(gcf, '-djpeg', outputfile);
return;
