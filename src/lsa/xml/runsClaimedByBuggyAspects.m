function runsClaimedByBuggyAspects() 
    load runsinfo.mat;
    load results.mat;
    Learn = configure();
    X = selectX(Xtru, Xobs, Learn); 
    % remove this sometime, eventually all experiments should generate an index
    % vector
    if not(exist('I', 'var'));
        I = 1:size(X,2);    
    end;
    buggyaspects = Learn.K - Learn.Kb + 1:Learn.K;
    [S, Is] = max(Pz_d(buggyaspects,:));
    Is = Is + Learn.K - Learn.Kb;
    for i = buggyaspects; 
        printruns(i, S, Is, I);
    end;

function printruns(i, S, Is, I);
    wins = find(Is == i & S ~= 0);
    xmlify([S(1,wins)' I(1,wins)'], {}, ['bugaspect_' num2str(i) '_runs.xml'], 'runs', 'run', {'Pz_d' 'index'}, {});
