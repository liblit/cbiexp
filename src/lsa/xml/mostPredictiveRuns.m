function mostPredictiveRuns()
    load runsinfo.mat;
    load results.mat;
    X = selectX(Xtru, Xobs, configure());
    % remove this sometime, eventually all experiments should generate an index
    % vector
    if not(exist('I', 'var'));
        I = 1:size(X,2);    
    end;
    numaspects = size(Pz_d, 1);
    doc = com.mathworks.xml.XMLUtils.createDocument('predictiveruns');
    docRoot = doc.getDocumentElement();
    for i = 1:numaspects;
        aspect = doc.createElement('aspect'); 
        printruns(doc, aspect, i, Pz_d, I);
        docRoot.appendChild(aspect);
    end;
    xmlwrite('predictive_runs.xml', doc);

function printruns(doc, node, i, X, I);
    [S, Is] = sortMinDiffs(X, i);
    pos = find(S > 0); 
    M = {num2cell(S(1,pos)') num2cell(I(Is(1,pos))')};
    xmlify(doc, node, M, 'run', {'score' 'index'});
