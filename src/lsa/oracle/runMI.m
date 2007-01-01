function runMI()
    load('runsinfo.mat');
    load('oracle.mat');
    MI = mutualInformation(X(:,Findices), Bugs(:,Findices));
    C = covariance(X(:,Findices), Bugs(:,Findices)); 
    sMI = sign(C).*MI;
    EB = shannon(Bugs(:,Findices));
    EP = shannon(X(:,Findices));
    Emin = min(repmat(EB', size(EP,1), 1), repmat(EP, 1, size(EB, 1)));
    nsMI = sMI .* spfun(inline('1./x'), Emin); 
    save -mat stats.mat MI C sMI nsMI EB EP 
    quit;

function C = covariance(X, Y)
    C = cov([Y' X']);
    % getting only the #preds by #bugs submatrix that we need
    C = C(linspace(size(Y,1) + 1,size(C,1), size(X,1)),linspace(1,size(Y,1),size(Y,1)));
