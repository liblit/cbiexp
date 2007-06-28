% function toDAformat(inputfile, outputfile)
%
% reads matlab data from inputfile, converts to a particular text format, and
% writes to outputfile
% inputfile is presumed to contain 
%  X -- a sparse matrix of counts(rows are predicates, columns are runs)
%  Findices -- a bit vector, a 1 indicates a failing run 
function toDAformat(inputfile, outputfile)
    load(inputfile);
    out = fopen(outputfile, 'w');
    for i = 1:size(X,2)
        fprintf(out, '%u ', Findices(i));    
        for j = 0:size(X,1) - 1
          fprintf(out, '%u:%u ', j, X(j + 1, i));
        end
        fprintf(out, '\n');
    end
    fclose(out);


