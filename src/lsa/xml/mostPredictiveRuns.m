function mostPredictiveRuns()
    load runsinfo.mat;
    load results.mat;
    X = selectX(Xtru, Xobs, configure());
    % remove this sometime, eventually all experiments should generate an index
    % vector
    if not(exist('I', 'var'));
        I = 1:size(X,2);    
    end;
    numaspects = size(Pz_d, 1);
    for i = 1:numaspects;
        printruns(i, Pz_d, I);
    end;

function printruns(i, X, I);
    [S, Is] = sortMinDiffs(X, i);
    pos = find(S > 0); 
    xmlify([S(1,pos)' I(Is(1,pos))'], {}, ['aspect_' num2str(i) '_exemplars.xml'], 'exemplars', 'run', {'score' 'index'}, {});
