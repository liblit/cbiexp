function runsClaimedByBuggyAspects() 
    load runsinfo.mat;
    load results.mat;
    Learn = configure();
    X = selectX(Xtru, Xobs, Learn); 
    % remove this sometime, eventually all experiments should generate an index
    % vector
    if not(exist('I', 'var'));
        I = 1:size(X,2);    
    end;
    buggyaspects = Learn.K - Learn.Kb + 1:Learn.K;
    [S, Is] = max(Pz_d(buggyaspects,:));
    Is = Is + Learn.K - Learn.Kb;
    doc = com.mathworks.xml.XMLUtils.createDocument('claimedruns');
    docRoot = doc.getDocumentElement(); 
    for i = buggyaspects; 
        aspect = doc.createElement('aspect');
        printruns(doc, aspect, i, S, Is, I);
        docRoot.appendChild(aspect);
    end;
    xmlwrite('bug_claimed_runs.xml', doc);

function printruns(doc, node, i, S, Is, I);
    wins = find(Is == i & S ~= 0);
    M = {num2cell(S(1,wins)') num2cell(I(1,wins)')};
    xmlify(doc, node, M, 'run', {'Pz_d' 'index'});
