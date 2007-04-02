function runsClaimedByAnyAspect() 
    load runsinfo.mat;
    load results.mat;
    Learn = configure();
    X = selectX(Xtru, Xobs, Learn); 
    [S, Is] = max(Pz_w);
    doc = com.mathworks.xml.XMLUtils.createDocument('claimedruns');
    docRoot = doc.getDocumentElement();
    for i = 1:Learn.K;
        aspect = doc.createElement('aspect');
        printfeatures(doc, aspect, i, S, Is);
        docRoot.appendChild(aspect);

        V(i) = numel(find(Is == i));
        Labels{i} = ['Aspect ' num2str(i) ' : ' num2str(V(i)) ' features']; 
        if i <= Learn.K - Learn.Kb;
            Explode(i) = 0;     
        else;
            Explode(i) = 1;
        end;
    end;
    xmlwrite('claimed_features.xml', doc);

    pie(V, Explode, Labels);
    print('-dpng', '-r300', 'claimed_features.png'); 
    quit();

function printfeatures(doc, node, i, S, Is, I);
    wins = find(Is == i);
    M = {num2cell(S(1,wins)') num2cell(wins')}; 
    xmlify(doc, node, M, 'run', {'Pz_w' 'index'});
