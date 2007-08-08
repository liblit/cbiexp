% outfile -- file to output summary
% srcdir -- the directory which holds the htmlified source of the experiment
function summarize(outfile, srcdir)
    doc = com.mathworks.xml.XMLUtils.createDocument('summary');
    docRoot = doc.getDocumentElement();
    docRoot.setAttribute('source-dir', srcdir);
    xmlwrite(outfile, doc);
