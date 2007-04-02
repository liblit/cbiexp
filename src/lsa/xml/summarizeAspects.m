function summarizeAspects()
    load runsinfo.mat
    printaspects(configure());
    quit()

function printaspects(Learn)
    doc = com.mathworks.xml.XMLUtils.createDocument('aspectinfos');
    docRoot = doc.getDocumentElement();
    for i = 1:Learn.K - Learn.Kb; 
        M{1,1}{i,1} = 'usage';
    end;
    for i = Learn.K - Learn.Kb + 1:Learn.K;
        M{1,1}{i,1} = 'buggy';
    end;
    xmlify(doc, docRoot, M, 'aspect', {'kind'});
    xmlwrite('aspects.xml', doc);

