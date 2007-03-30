function runsClaimedByAnyAspect() 
    load runsinfo.mat;
    load results.mat;
    Learn = configure();
    X = selectX(Xtru, Xobs, Learn); 
    % remove this sometime, eventually all experiments should generate an index
    % vector
    if not(exist('I', 'var'));
        I = 1:size(X,2);    
    end;
    [S, Is] = max(Pz_d);
    for i = 1:Learn.K;
        printruns(i, S, Is, I);
    end;

function printruns(i, S, Is, I);
    wins = find(Is == i);
    xmlify([S(1,wins)' I(1,wins)'], {}, ['aspect_' num2str(i) '_runs.xml'], 'runs', 'run', {'Pz_d' 'index'}, {});
