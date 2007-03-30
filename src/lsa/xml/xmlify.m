% X -- matrix to xmlify, rows correspond to elements, columns give values of
% attributes
% Y -- cell array to xmlify, necessary when we use strings rather than numeric
% values
% filename -- name of file to write xml to, as string
% t -- name for top (enclosing) element as string
% e -- name for each contained element as string
% A -- array of string cells containing attributes for X 
% A -- array of string cells containing attributes for Y
function xmlify(X,Y,filename,t,e,A,B)
    % checking arguments
    if not(iscellstr(A) & iscellstr(B) & iscellstr(Y));
        error('A, B, and Y must be arrays of string cells.');  
    end;
    if not(size(X,2) == numel(A));  
        error('A must have the same number of attributes as X has columns.');
    end;
    if not(size(Y,2) == numel(B));  
        error('B must have the same number of attributes as Y has columns.');
    end;
    if not(ischar(filename) & ischar(t) & ischar(e));
        error('filename, t, and e, must be string arrays');
    end; 
    if not(size(X,1) == size(Y,1) | numel(X) == 0 | numel(Y) == 0);
        error('X and Y represent the same elements, so must have the same number of rows.');
    end;

    doc = com.mathworks.xml.XMLUtils.createDocument(t);
    docRoot = doc.getDocumentElement();
    for i = 1:size(X,1);
        element = doc.createElement(e);
        for j = 1:size(X,2);
            element.setAttribute(A(j), num2str(X(i,j)));
        end;
        for j = 1:size(Y,2);
            element.setAttribute(B(j), Y{i,j});
        end;
        docRoot.appendChild(element);
    end;
    xmlwrite(filename, doc);
    

