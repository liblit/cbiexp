function Results = deriveProbabilities(Results)
    Results.Pw = marginalize(Results.Pw_z, Results.Pz);
    Results.Pd = marginalize(Results.Pd_z, Results.Pz);
    Results.Pz_d = applyBayes(Results.Pd_z,Results.Pz,Results.Pd);
    Results.Pz_w = applyBayes(Results.Pw_z,Results.Pz,Results.Pw); 
    Results.Pd_w = Results.Pd_z * Results.Pz_w;
    Results.Pw_d = Results.Pw_z * Results.Pz_d; 
