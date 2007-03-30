function summarize()
    load results.mat;
    load runsinfo.mat;
    load clusters.mat;
    X = selectX(Xtru, Xobs, configure());
    doc = com.mathworks.xml.XMLUtils.createDocument('plsa');
    docRoot = doc.getDocumentElement();
    docRoot.setAttribute('source-dir', '../../../../src');
    xmlwrite('summary.xml', doc);
    quit();
