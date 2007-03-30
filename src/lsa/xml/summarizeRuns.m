function summarizeRuns()
    load runsinfo.mat;
    Learn = configure();
    printruns(Learn, Findices, Sindices, selectX(Xtru, Xobs, Learn));
    quit;

function o = outcome(F, S, i)
    if F;
        o = 'failure';
    elseif S;
        o = 'success';
    else
        o = 'ignore';
    end;

function O = outcomeVector(Fs,Ss)
    for i = 1:numel(Fs);
      O{i,1} = outcome(Fs(i), Ss(i), i);
    end;

function printruns(Learn, Findices, Sindices, X);
    numruns = size(X,2);
    Fs(1,numruns) = 0;
    Fs(Findices) = 1;
    Ss(1, numruns) = 0;
    Ss(Sindices) = 1;
    xmlify(sum(X,1)', outcomeVector(Fs,Ss), 'runs.xml', 'runinfos', 'run', {'totalcount'}, {'outcome'});
