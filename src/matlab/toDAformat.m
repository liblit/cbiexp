% function toDAformat(inputfile, outputfile)
%
% reads matlab data from inputfile, converts to a particular text format, and
% writes to outputfile
function toDAformat(inputfile, outputfile)
    load(inputfile);

    X = full(Data.X);
    Fvector = Data.Fvector;

    out = fopen(outputfile, 'w');
    for i = 1:size(X,2)
        fprintf(out, '%u ', Fvector(i));    
        for j = 0:size(X,1) - 1
          fprintf(out, '%u:%u ', j, X(j + 1, i));
        end
        fprintf(out, '\n');
    end
    fclose(out);


