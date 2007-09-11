function makeBaseFiles(infilename)
    load(infilename)
    for i = 1:Results.Learn.numaspects; 
        fid = fopen(['aspect_' num2str(i) '.base'], 'w');
        fprintf(fid, '%u', i);
        fclose(fid);
    end;
