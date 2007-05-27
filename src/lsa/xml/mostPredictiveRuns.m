function mostPredictiveRuns()
    load runsinfo.mat;
    load results.mat;
    Indexing = indexing();
    numaspects = size(Pz_d, 1);
    totalcounts = sum(X,1); 
    doc = com.mathworks.xml.XMLUtils.createDocument('predictiveruns');
    docRoot = doc.getDocumentElement();
    for i = 1:numaspects;
        aspect = doc.createElement('aspect'); 
        printruns(doc, aspect, i, Pz_d, Indices, Indexing.offset, totalcounts);
        docRoot.appendChild(aspect);
    end;
    xmlwrite('predictive_runs.xml', doc);

function printruns(doc, node, i, X, Indices, offset, totalcounts);
    [S, Is] = sortMinDiffs(X, i);
    pos = find(S > 0); 
    M = {num2cell(S(1,pos)') num2cell(Indices(Is(1,pos))' + offset) num2cell(totalcounts(Is(1,pos))')};
    xmlify(doc, node, M, 'run', {'score' 'index' 'totalcounts'});

    figure;
    plot(S, 'o');
    xlabel('Rank');
    ylabel(['P(z_{' num2str(i) '}|d_{i}) - max \{ P(z_{k}|d_{i}) | k <> ' num2str(i) '\}']);
    print('-dpng', '-r300', ['aspect_' int2str(i) '_predictive_runs.png']);
