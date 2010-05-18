function convertBugsInfo(datafilename, inputfilename, outfilename)
    load(datafilename)
    Bugs = int8(load(inputfilename));
    Bugs = Bugs(Data.Indices)
    save('-mat', outfilename, 'Bugs')
