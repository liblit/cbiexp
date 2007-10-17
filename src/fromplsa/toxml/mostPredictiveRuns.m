function mostPredictiveRuns(resultsfile, datafile, outputfile)
    load(resultsfile);
    load(datafile);
    numaspects = Results.Learn.numaspects;
    Pz_d = Results.Pz_d;
    doc = com.mathworks.xml.XMLUtils.createDocument('predictiveruns');
    docRoot = doc.getDocumentElement();
    for i = 1:numaspects;
        aspect = doc.createElement('aspect'); 
        printruns(doc, aspect, i, Pz_d); 
        docRoot.appendChild(aspect);
    end;
    xmlwrite(outputfile, doc);

function printruns(doc, node, i, X, Indices, offset);
    [S, Is] = sortMinDiffs(X, i);
    pos = find(S > 0); 
    M = {num2cell(S(1,pos)') num2cell(Is(1,pos)')};
    xmlify(doc, node, M, 'run', {'score' 'index'});

    figure;
    plot(S, 'o');
    xlabel('Rank');
    ylabel(['P(z_{' num2str(i) '}|d_{i}) - max \{ P(z_{k}|d_{i}) | k <> ' num2str(i) '\}']);
    print('-dpng', '-r300', ['aspect_' int2str(i) '_predictive_runs.png']);
