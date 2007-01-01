function E = shannon(X)
    if nargin < 1
        error('shannon requires an argument');
    end;
    
    if not(issparse(X))
        X = sparse(X);
    end;

    X = spones(X);
    s = size(X,2);
    Xt = sum(X,2)/s;
    Xf = sum(not(X),2)/s;
    E = Xt .* spfun(inline('log2(1./x)'), Xt) + Xf .* spfun(inline('log2(1./x)'), Xf);
