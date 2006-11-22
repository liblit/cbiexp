function sortFeatures(Pw_z, curdir)
    [Y,I] = sort(Pw_z, 1,'descend');
    outfile = [curdir '/sorted_features.txt'];
    dlmwrite(outfile, I, 'delimiter', '\t'); 
return;
