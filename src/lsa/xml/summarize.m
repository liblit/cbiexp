function summarize()
    load results.mat;
    load runsinfo.mat;
    load clusters.mat;
    Learn = configure();
    fid = fopen('rawsummary.xml', 'wt');
    fprintf(fid, '<?xml version=\"1.0\"?>\n');
    fprintf(fid, '<!DOCTYPE plsa SYSTEM "rawsummary.dtd">\n'); 
    fprintf(fid, '<plsa>\n');
    printaspects(fid, Learn, Clusters, Pw_z);
    printfeatures(fid, size(Pw_z,1));
    printruns(fid, Learn, Findices, Sindices, UsageClusters, BugClusters);
    fprintf(fid, '</plsa>\n');
    fclose(fid);
    quit;

function printaspects(fid, Learn, Clusters,Pw_z)  
   for i = 1:Learn.K; 
       if i <= Learn.K - Learn.Kb; 
           kind = 'usage';
       else
           kind = 'bug';
       end;
       [S,I] = sort(Pw_z(:,i), 1, 'descend');
       fprintf(fid, '<aspect kind=\"%s\" ratio=\"%s\">\n', kind, sum(S(1:10)));
       for j = find(Clusters(i,:) > 0);
           fprintf(fid, '<runid index=\"%u\"/>', j); 
       end;
       for j = 1:10;
           fprintf(fid, '<featureclaimed index=\"%u\" probability=\"%0.4f\"/>', I(j), S(j));
       end;
       fprintf(fid, '</aspect>\n');
   end;

function printfeatures(fid, numfeatures)
    for i = 1:numfeatures;
        fprintf(fid, '<feature infoindex=\"%u\"/>\n', i);
    end;

function printruns(fid, Learn, Findices, Sindices, UsageClusters, BugClusters) 
    numruns = size(UsageClusters, 2);
    Fs(1, numruns) = 0;
    Fs(Findices) = 1;
    Ss(1, numruns) = 0;
    Ss(Sindices) = 1;
    for i = 1:numruns;
        if Fs(i);
            outcome='failure';
        elseif Ss(i);
            outcome='success';
        else
            outcome='ignore'; 
        end;
        fprintf(fid, '<run outcome=\"%s\">\n', outcome);
        usageaspect = find(UsageClusters(:,i));
        usageprob = UsageClusters(usageaspect,i);
        if not(isempty(usageaspect)) & usageprob > 0;
            fprintf(fid, '<usageaspect index=\"%u\" probability=\"%0.4f\"/>', usageaspect, usageprob);
        end;
        if outcome == 'failure';
            buggyaspect = find(BugClusters(:,i));
            buggyprob = BugClusters(buggyaspect,i);
            if not(isempty(buggyaspect)) & buggyprob > 0;
                fprintf(fid, '<buggyaspect index=\"%u\" probability=\"%0.4f\"/>', buggyaspect + Learn.K - Learn.Kb , buggyprob);
            end;
        end;
        fprintf(fid, '</run>\n');
    end;
