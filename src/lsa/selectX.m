function X = selectX(Xtru, Xobs, Learn)
    if Learn.TruthCounts;
        X = Xtru;
    else;
        X = Xobs;
    end;
