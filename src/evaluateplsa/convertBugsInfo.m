function convertBugsInfo(datafilename, inputfilename, outfilename, oracletype)
    load(datafilename)
    Bugs = load(inputfilename);
    if strncmp(oracletype, 'Recorded', 8);
        Bugs = convertInfo(Bugs, Data);
    elseif strncmp(oracletype, 'Synthesized', 11);
        Bugs = convertInfo(Bugs, Data, 1); 
    elseif strncmp(oracletype, 'None', 4);
        error('This experiment has only one bug, so evaluating by RandIndex is pointless.')
    else
        error('Unknown oracle type.')
    end
       
    save('-mat', outfilename, 'Bugs')
