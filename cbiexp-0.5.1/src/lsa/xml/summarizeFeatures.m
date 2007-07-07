function summarizeFeatures()
    load runsinfo.mat;
    printfeatures(X);

function printfeatures(X);
    doc = com.mathworks.xml.XMLUtils.createDocument('featureinfos');
    docRoot = doc.getDocumentElement();
    xmlify(doc, docRoot, {num2cell(sum(X,2))}, 'feature', {'totalcount'});
    xmlwrite('features.xml', doc);
