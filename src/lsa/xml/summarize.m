function summarize()
    load results.mat;
    load runsinfo.mat;
    load clusters.mat;
    Learn = configure();
    if Learn.TruthCounts;
        X = Xtru;
    else;
        X = Xobs;
    end;
    if Learn.Normalized;
        NX = normalizeColumns(X);
    else;
        NX = X;
    end;
    [I,J,V] = find(NX);
    fid = fopen('summary.xml', 'wt');
    fprintf(fid, '<?xml version=\"1.0\"?>\n');
    fprintf(fid, '<!DOCTYPE plsa SYSTEM "summary.dtd">\n'); 
    fprintf(fid, '<plsa source-dir="../../../../src">\n');
    printaspects(fid, Learn, Clusters, Pw_z,X,Pz_d);
    fprintf(fid, '</plsa>\n');
    fclose(fid);
    printfeatures(Pw_z);
    printruns(Learn, Findices, Sindices, UsageClusters, BugClusters, X);
    quit;

function printclaimedruns(fid, aspectindex, runindices, Pz_d) 
    Pz_d = Pz_d(:, runindices);
    [Sd, Id] = sortMinDiffs(Pz_d, aspectindex); 
    Pz_d = Pz_d(aspectindex,Id);
    for j = 1:numel(runindices);
       index = runindices(Id(1,j));
       fprintf(fid, '<runid index=\"%u\" Pz_d-diff=\"%0.4f\" Pz_d-abs=\"%0.4f\"/>', index, Sd(1, j), Pz_d(1, j));
    end;
    return; 

function printaspects(fid, Learn, Clusters,Pw_z,X,Pz_d)
   topnum = 400;
   Sum = sum(X,1);
   numaspects = Learn.K;
   numbugaspects = Learn.Kb;
   for i = 1:numaspects;
       runindices = find(Clusters(i,:));
       if i <= numaspects - numbugaspects; 
           kind = 'usage';
       else
           kind = 'bug';
       end;
       [S,I] = sort(Pw_z(:,i), 1, 'descend');
       counts = full(Sum(runindices));
       fprintf(fid, '<aspect index=\"%u\" kind=\"%s\" ratio=\"%1.4f\" maxrunlength=\"%u\" minrunlength=\"%u\" meanrunlength=\"%u\" runlengthstd=\"%u\">\n', i, kind, sum(S(1:topnum)),max(counts),min(counts),round(mean(counts)),round(std(counts)));
       printclaimedruns(fid, i, runindices, Pz_d);
       for j = 1:topnum;
           featureindex = I(j);
           fprintf(fid, '<featureclaimed index=\"%u\"/>', featureindex);
       end;
       for k = 1:numaspects;
           vals = Pz_d(k,runindices); 
           fprintf(fid, '<aspectprobability aspectindex=\"%u\" mean=\"%0.4f\" std=\"%0.4f\" median=\"%0.4f" max=\"%0.4f\" min=\"%0.4f\"/>', k, mean(vals), std(vals), median(vals), max(vals), min(vals)); 
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
