function summarize()
    doc = com.mathworks.xml.XMLUtils.createDocument('summary');
    docRoot = doc.getDocumentElement();
    docRoot.setAttribute('source-dir', '../../../../src');
    xmlwrite('summary.xml', doc);
