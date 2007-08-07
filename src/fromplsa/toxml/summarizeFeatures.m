function summarizeFeatures(inputfile, outputfile)
    load(inputfile);
    printfeatures(outputfile, Data.X);

function printfeatures(outputfile, X);
    doc = com.mathworks.xml.XMLUtils.createDocument('featureinfos');
    docRoot = doc.getDocumentElement();
    xmlify(doc, docRoot, {num2cell(sum(X,2))}, 'feature', {'sumoverdocs'});
    xmlwrite(outputfile, doc);
