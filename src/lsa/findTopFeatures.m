function findTopFeatures()
    load runsinfo.mat
    load results.mat
    numfeatures = 10;
    [S,I] = sort(Pw_z,1,'descend');
    for i = 1:size(Pw_z,2);  
        out = fopen(strcat('aspect', int2str(i), '.txt'),'w');
        for j = 1:numfeatures;        
            fprintf(out, '%u\t%0.4f\n', I(j,i), S(j,i));
        end;
        fclose(out);
    end; 
    quit
        

