% doc -- xml document node (Java) 
% node -- node to add elements to (Java)
% X -- cell array to xmlify, rows correspond to elements, columns give values of
% attributes, we use a cell array in case we need to handle string values
% e -- name for each contained element as string
% A -- array of string cells containing attributes for X 
% xmlify works entirely by side-effects, inserting elements into node 
function xmlify(doc, node, X, e, A)
    % checking arguments
    if not(iscellstr(A));
        error('A must be an array of string cells.');  
    end;
    if not(size(X,2) == numel(A));  
        error('A must have the same number of attributes as X has columns.');
    end;
    if not(ischar(e));
        error('e must be string array');
    end; 

    numrows = size(X{1,1},1);
    numattributes = size(X,2); 

    for i = 1:numrows;
        element = doc.createElement(e);
        for j = 1:numattributes;
            element.setAttribute(A(j), num2str(X{1,j}{i,1}));
        end;
        node.appendChild(element);
    end;
