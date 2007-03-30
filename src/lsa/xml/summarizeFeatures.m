function summarizeFeatures()
    load runsinfo.mat;
    printfeatures(selectX(Xtru, Xobs, configure()));
    quit;

function printfeatures(X);
    xmlify(sum(X,2), {}, 'features.xml', 'featureinfos', 'feature', {'totalcount'}, {});
