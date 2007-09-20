function makeBaseFiles(infilename, which)
    load(infilename)
    if (which == 'all');
        for i = 1:Results.Learn.numaspects; 
            fid = fopen(['aspect_' num2str(i) '.base'], 'w');
            fprintf(fid, '%u', i);
            fclose(fid);
        end;
    elseif (which == 'buggy'); 
        for i = Results.Learn.BuggyIndices; 
            fid = fopen(['aspect_' num2str(i) '.base'], 'w');
            fprintf(fid, '%u', i);
            fclose(fid);
        end
    else
        error('Specified base file indices are not recognized.')
    end
