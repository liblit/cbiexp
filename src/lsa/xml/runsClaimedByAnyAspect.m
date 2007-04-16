function runsClaimedByAnyAspect() 
    load runsinfo.mat;
    load results.mat;
    Learn = configure();
    [S, Is] = max(Pz_d);
    doc = com.mathworks.xml.XMLUtils.createDocument('claimedruns');
    docRoot = doc.getDocumentElement();
    for i = 1:Learn.K;
        aspect = doc.createElement('aspect');
        printruns(doc, aspect, i, S, Is, Indices);
        docRoot.appendChild(aspect);

        V(i) = numel(find(Is == i));
        Labels{i} = ['Aspect ' num2str(i) ' : ' num2str(V(i)) ' runs']; 
        if i <= Learn.K - Learn.Kb;
            Explode(i) = 0;     
        else;
            Explode(i) = 1;
        end;
    end;
    xmlwrite('claimed_runs.xml', doc);

    pie(V, Explode, Labels);
    print('-dpng', '-r300', 'claimed_runs.png'); 
    quit();

function printruns(doc, node, i, S, Is, Indices);
    wins = find(Is == i);
    M = {num2cell(S(1,wins)') num2cell(Indices(1,wins)')}; 
    xmlify(doc, node, M, 'run', {'Pz_d' 'index'});
