function makeBaseFiles(infilename, which)
    load(infilename)
    if (strncmp(which, 'all', max(numel(which), numel('all'))) == 1)
        for i = 1:Results.Learn.numaspects; 
            fid = fopen(['aspect_' num2str(i) '.base'], 'w');
            fprintf(fid, '%u', i);
            fclose(fid);
        end;
    elseif (strncmp(which, 'buggy', max(numel(which), numel('buggy'))) == 1);
        for i = Results.Learn.BuggyIndices; 
            fid = fopen(['aspect_' num2str(i) '.base'], 'w');
            fprintf(fid, '%u', i);
            fclose(fid);
        end
    else
        error('Specified base file indices are not recognized.')
    end
