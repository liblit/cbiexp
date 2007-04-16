function summarizeRuns()
    load runsinfo.mat;
    printruns(Findices, Sindices, X); 
    quit;

function o = outcome(F, S, i)
    if F;
        o = 'failure';
    elseif S;
        o = 'success';
    else
        o = 'ignore';
    end;

function O = outcomeVector(Fs,Ss)
    for i = 1:numel(Fs);
      O{i,1} = outcome(Fs(i), Ss(i), i);
    end;

function printruns(Findices, Sindices, X);
    numruns = size(X,2);
    Fs(1,numruns) = 0;
    Fs(Findices) = 1;
    Ss(1, numruns) = 0;
    Ss(Sindices) = 1;
    doc = com.mathworks.xml.XMLUtils.createDocument('runinfos');
    docRoot = doc.getDocumentElement();
    M = {outcomeVector(Fs, Ss) num2cell(sum(X,1)')};
    xmlify(doc, docRoot, M, 'run', {'outcome' 'totalcount'});
    xmlwrite('runs.xml', doc);
