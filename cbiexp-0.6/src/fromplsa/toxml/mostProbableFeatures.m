function mostProbableFeatures(inputfile, outputfile)
    load(inputfile);
    numaspects = Results.Learn.numaspects; 
    Pw_z = Results.Pw_z;
    doc = com.mathworks.xml.XMLUtils.createDocument('probablefeatures');
    docRoot = doc.getDocumentElement();
    for i = 1:numaspects;
        aspect = doc.createElement('aspect');
        printfeatures(doc, aspect, i, Pw_z(:,i));
        docRoot.appendChild(aspect); 
    end;
    xmlwrite(outputfile, doc);

function printfeatures(doc, node, i, X)
    [S, I] = sort(X, 1, 'descend');
    M = {num2cell(S) num2cell(I)};
    xmlify(doc, node, M, 'feature', {'Pw_z' 'index'});

    figure;
    plot(S, 'o');
    xlabel('Rank');
    ylabel(['P(w_{i}|z_{' num2str(i) '})']);
    print('-dpng', '-r300', ['aspect_' int2str(i) '_probable_features.png']);
