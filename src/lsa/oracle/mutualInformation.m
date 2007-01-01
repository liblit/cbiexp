function MI = mutualInformation(X,Y)

    if nargin < 2
        error('mutualInformation requires two arguments to be mutual');
    end;
    
    if not(issparse(X))
        X = sparse(X);
    end;
    
    if not(issparse(Y))
        Y = sparse(Y);
    end;
    
    X = spones(X);
    Y = spones(Y);
    
    s = size(X,2);
    
    TT = full((X * Y')./s);
    TF = full((X * not(Y'))./s);
    FT = full((not(X) * Y')./s);
    FF = full((not(X) * spones(not(Y')))./s);
    
    Xt = sum(X,2)./s;
    Xf = sum(not(X),2)./s;
    
    Yt = sum(Y,2)./s;
    Yf = sum(not(Y),2)./s;
    
    MI = doPart(TT, Xt, Yt) + doPart(TF, Xt, Yf) + doPart(FT, Xf, Yt) + doPart(FF, Xf, Yf);
    
return

function X = doPart(V,L,R)
    X = diag(spfun(inline('1./x'), L)) * V * diag(spfun(inline('1./x'), R));
    X = spfun(@log2,X) .* V;
return
