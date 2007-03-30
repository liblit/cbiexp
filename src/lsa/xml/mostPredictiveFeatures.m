function mostPredictiveFeatures()
    load runsinfo.mat;
    load results.mat;
    X = selectX(Xtru, Xobs, configure());
    numaspects = size(Pz_w, 1);
    for i = 1:numaspects;
        printfeatures(i, Pz_w);
    end;

function printfeatures(i, X);
    [S, I] = sortMinDiffs(X, i);
    pos = find(S > 0); 
    xmlify([S(1,pos)' I(1,pos)'], {}, ['aspect_' num2str(i) '_predictors.xml'], 'predictors', 'feature', {'score' 'index'}, {});
