function mostPredictiveFeatures(resultsfile, outputfile)
    load(resultsfile)
    numaspects = Results.Learn.numaspects;
    Pz_w = Results.Pz_w;
    doc = com.mathworks.xml.XMLUtils.createDocument('predictivefeatures');
    docRoot = doc.getDocumentElement();
    for i = 1:numaspects;
        aspect = doc.createElement('aspect');
        printfeatures(doc, aspect, i, Pz_w);
        docRoot.appendChild(aspect); 
    end;
    xmlwrite(outputfile, doc);

function printfeatures(doc, node, i, X);
    [S, I] = sortMinDiffs(X, i);
    pos = find(S > 0); 
    M = {num2cell(S(1,pos)') num2cell(I(1,pos)')};
    xmlify(doc, node, M, 'feature', {'score' 'index'});

    figure;
    plot(S, 'o');
    xlabel('Rank');
    ylabel(['P(z_{' num2str(i) '}|w_{i}) - max \{ P(z_{k}|w_{i}) | k <> ' num2str(i) '\}']);
    print('-dpng', '-r300', ['aspect_' int2str(i) '_predictive_features.png']);
