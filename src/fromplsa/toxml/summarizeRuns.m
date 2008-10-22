% inputfile --- data on which the plsa analysis was run
% analysisfile --- original data, unnormalized and uncut
% outputﬁile --- where we store the summary
function summarizeRuns(inputfile, analysisfile, outputfile)
    load(analysisfile)
    X = Data.X;
    clear Data;
    load(inputfile)
    printruns(outputfile, Data.Indices, Data.Findices, Data.Sindices, X); 

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

function printruns(outputfile, Indices, Findices, Sindices, OldX);
    numruns = numel(Indices); 
    Fs(1,numruns) = 0;
    Fs(find(Findices(Indices))) = 1;
    Ss(1, numruns) = 0;
    Ss(find(Sindices(Indices))) = 1;
    OldX = OldX(:, Indices);
    doc = com.mathworks.xml.XMLUtils.createDocument('runinfos');
    docRoot = doc.getDocumentElement();
    M = {outcomeVector(Fs, Ss) num2cell(Indices - 1) num2cell(sum(OldX,1)')};
    xmlify(doc, docRoot, M, 'run', {'outcome' 'runid' 'sum'});
    xmlwrite(outputfile, doc);
