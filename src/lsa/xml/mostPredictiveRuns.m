function mostPredictiveRuns()
    load runsinfo.mat;
    load results.mat;
    numaspects = size(Pz_d, 1);
    doc = com.mathworks.xml.XMLUtils.createDocument('predictiveruns');
    docRoot = doc.getDocumentElement();
    for i = 1:numaspects;
        aspect = doc.createElement('aspect'); 
        printruns(doc, aspect, i, Pz_d, Indices);
        docRoot.appendChild(aspect);
    end;
    xmlwrite('predictive_runs.xml', doc);

function printruns(doc, node, i, X, Indices);
    [S, Is] = sortMinDiffs(X, i);
    pos = find(S > 0); 
    M = {num2cell(S(1,pos)') num2cell(Indices(Is(1,pos))')};
    xmlify(doc, node, M, 'run', {'score' 'index'});
