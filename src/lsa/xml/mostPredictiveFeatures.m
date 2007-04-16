function mostPredictiveFeatures()
    load runsinfo.mat;
    load results.mat;
    numaspects = size(Pz_w, 1);
    doc = com.mathworks.xml.XMLUtils.createDocument('predictivefeatures');
    docRoot = doc.getDocumentElement();
    for i = 1:numaspects;
        aspect = doc.createElement('aspect');
        printfeatures(doc, aspect, i, Pz_w);
        docRoot.appendChild(aspect); 
    end;
    xmlwrite('predictive_features.xml', doc);
    quit();

function printfeatures(doc, node, i, X);
    [S, I] = sortMinDiffs(X, i);
    pos = find(S > 0); 
    M = {num2cell(S(1,pos)') num2cell(I(1,pos)')};
    xmlify(doc, node, M, 'feature', {'score' 'index'});
