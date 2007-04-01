function runsClaimedByAnyAspect() 
    load runsinfo.mat;
    load results.mat;
    Learn = configure();
    X = selectX(Xtru, Xobs, Learn); 
    % remove this sometime, eventually all experiments should generate an index
    % vector
    if not(exist('I', 'var'));
        I = 1:size(X,2);    
    end;
    [S, Is] = max(Pz_d);
    doc = com.mathworks.xml.XMLUtils.createDocument('claimedruns');
    docRoot = doc.getDocumentElement();
    for i = 1:Learn.K;
        aspect = doc.createElement('aspect');
        printruns(doc, aspect, i, S, Is, I);
        docRoot.appendChild(aspect);
    end;
    xmlwrite('claimed_runs.xml', doc);

function printruns(doc, node, i, S, Is, I);
    wins = find(Is == i);
    M = {num2cell(S(1,wins)') num2cell(I(1,wins)')}; 
    xmlify(doc, node, M, 'run', {'Pz_d' 'index'});
