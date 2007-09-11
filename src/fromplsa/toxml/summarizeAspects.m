function summarizeAspects(infilename, outfilename)
    load(infilename)  
    printaspects(outfilename, Results.Learn)

function printaspects(outfilename, Learn)
    doc = com.mathworks.xml.XMLUtils.createDocument('aspectinfos');
    docRoot = doc.getDocumentElement();
    for i = 1:Learn.numaspects;
        if any(find(i == Learn.UsageIndices));
            M{1,1}{i,1} = 'usage';
        elseif any(find(i == Learn.BuggyIndices));
            M{1,1}{i,1} = 'buggy';
        else
            error ('Aspect is not labeled');
        end;
    end;
    xmlify(doc, docRoot, M, 'aspect', {'kind'});
    xmlwrite(outfilename, doc);

