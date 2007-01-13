function summarize()
    load results.mat;
    load runsinfo.mat;
    load clusters.mat;
    Learn = configure();
    fid = fopen('summary.xml', 'wt');
    fprintf(fid, '<?xml version=\"1.0\"?>\n');
    fprintf(fid, '<!DOCTYPE plsa SYSTEM "summary.dtd">\n'); 
    fprintf(fid, '<plsa>\n');
    printaspects(fid, Learn, Clusters, Pw_z,X);
    fprintf(fid, '</plsa>\n');
    fclose(fid);
    printfeatures(Pw_z);
    printruns(Learn, Findices, Sindices, UsageClusters, BugClusters, X);
    quit;

function printaspects(fid, Learn, Clusters,Pw_z,X)  
   topnum = 400;
   Sum = sum(X,1);
   for i = 1:Learn.K; 
       if i <= Learn.K - Learn.Kb; 
           kind = 'usage';
       else
           kind = 'bug';
       end;
       [S,I] = sort(Pw_z(:,i), 1, 'descend');
       counts = full(Sum(find(Clusters(i,:))));
       fprintf(fid, '<aspect index=\"%u\" kind=\"%s\" ratio=\"%1.4f\" maxrunlength=\"%u\" minrunlength=\"%u\" meanrunlength=\"%u\" runlengthstd=\"%u\">\n', i, kind, sum(S(1:topnum)),max(counts),min(counts),round(mean(counts)),round(std(counts)));
       for j = find(Clusters(i,:) > 0);
           fprintf(fid, '<runid index=\"%u\"/>', j); 
       end;
       for j = 1:topnum;
           fprintf(fid, '<featureclaimed index=\"%u\"/>', I(j));
       end;
       fprintf(fid, '</aspect>\n');
   end;

function printfeatures(Pw_z)
    [S,I] = sort(Pw_z, 'descend');
    [V,J] = sort(I, 'ascend');
    fid = fopen('features.xml', 'wt'); 
    fprintf(fid, '<?xml version=\"1.0\"?>\n');
    fprintf(fid, '<!DOCTYPE featureinfos SYSTEM "features.dtd">\n'); 
    fprintf(fid, '<featureinfos>\n');
    for i = 1:size(Pw_z,1);
      fprintf(fid, '<feature>\n');
      printaspectentrys(fid, Pw_z(i,:), J(i,:));
      fprintf(fid, '</feature>\n');
    end;
    fprintf(fid, '</featureinfos>\n');
    fclose(fid);

function printaspectentrys(fid, probs, rank)
    for i = 1:numel(probs);
      fprintf(fid, '<aspectentry aspectindex=\"%u\" rank=\"%u\" probability=\"%0.4f\"/>\n', i, rank(i), probs(i));
    end;

function printruns(Learn, Findices, Sindices, UsageClusters, BugClusters,X)
    fid = fopen('runs.xml', 'wt'); 
    fprintf(fid, '<?xml version=\"1.0\"?>\n');
    fprintf(fid, '<!DOCTYPE runinfos SYSTEM "runs.dtd">\n'); 
    fprintf(fid, '<runinfos>\n');
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
        fprintf(fid, '<run outcome=\"%s\" totalcount=\"%u\">\n', outcome, sum(full(X(:,i)),1));
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
    fprintf(fid, '</runinfos>\n'); 
    fclose(fid);
