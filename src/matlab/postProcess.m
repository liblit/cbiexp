% function postProcess(inputfile, outputfile)
%
% Does any post processing necessary, beyond removing discarded runs.
% Performs actions in order specified.

function postProcess(inputfile, outputfile, flagsfile)
    load(inputfile);

    flags = xmlread(flagsfile);
    actions = flags.getDocumentElement().getElementsByTagName('action');

    numactions = actions.getLength();

    for i = 0:numactions - 1 
        action = actions.item(i);
        name = char(action.getAttribute('name'));
        callstring = ['Data = ' name '(Data'];

        args = action.getElementsByTagName('arg'); 
        numargs = args.getLength();
        for j = 0:numargs - 1
            arg = args.item(j);
            callstring = [callstring ', ' char(arg.getAttribute('value'))];
        end
        callstring = [callstring ');'];
        eval(callstring);
    end

    save('-mat', outputfile, 'Data');
