% function [Pw_z,Pd_z,Pz,Li] = pLSA_EM(X,Learn,Sindices)
%
% Probabilistic Latent semantic alalysis (pLSA)
%
% Notation:
% X ... (m x nd) term-document matrix (observed data)
%       X(i,j) stores number of occurrences of word i in document j
%
% m  ... number of words (vocabulary size)
% nd ... number of documents
% K  ... number of topics
%
% Learn ... Structure holding all settings 
%
% Sindices ... the indices of all succeeding runs in the model
%
% Li   ... likelihood for each iteration
% Pz   ... P(z)
% Pd_z ... P(d|z) 
% Pw_z ... P(w|z) corresponds to beta parameter in LDA
% Pz_d ... P(z|d) 
%
% Pz_wd ... P(z|w,d) posterior on z
%
% 
% References: 
% [1] Thomas Hofmann: Probabilistic Latent Semantic Analysis, 
% Proc. of the 15th Conf. on Uncertainty in Artificial Intelligence (UAI'99) 
%
% mulhern 
% 
% Loosely inspired by some code distributed at
% http://people.csail.mit.edu/fergus/iccv2005/bagwords.html 
% as part of a short course on recognizing and learning object categories.

function [Pw_z,Pd_z,Pz,Li] = pLSA_EM(X,Learn,Sindices)

if nargin < 2
    error('pLSA_EM called with incomplete specification.');
end;

if Learn.Kb > 0
    if nargin < 3
        error('Learn calls for constrained analysis but indices of succeedig runs not passed to pLSA_EM');
    end;

    if Learn.K < 1
        error('Can not have 0 or negative number of aspects');
    end

    if Learn.Kb >= Learn.K 
        error('Can not have the same or more bug aspects than there are total aspects'); 
    end;

    Kb = Learn.Kb;
    K = Learn.K;
    Knb = K - Kb;
    KbIndices = find([diag(zeros(Knb))' diag(ones(Kb))']);  
else
    KbIndices = [];
    Sindices = [];
end


m  = size(X,1); % vocabulary size
nd = size(X,2); % # of documents
e = nnz(X); 

[I,J,V] = find(X);

% initialize Pz, Pd_z,Pw_z
[Pz,Pd_z,Pw_z] = pLSA_init(m,nd,K,Sindices,KbIndices);

Li    = [];
maxit = Learn.Max_Iterations;

% EM algorithm
for it = 1:maxit   
   
   % E-step
   Pz_dw = pLSA_Estep(Pw_z,Pd_z,Pz,I,J,K,m,nd,e);
   
   % M-step
   [Pw_z,Pd_z,Pz] = pLSA_Mstep(X,Pz_dw,K,Sindices,KbIndices);

   % Evaluate data log-likelihood
   Li(it) = pLSA_logL(I,J,V,Pw_z,Pz,Pd_z,m,nd,e);   
      
   % convergence?
   dLi = 0;
   if it > 1
       dLi = Li(it) - Li(it-1);
       if dLi < Learn.Min_Likelihood_Change, break; end;   
   end;
end;

return;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% initialize conditional probabilities for EM 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function [Pz,Pd_z,Pw_z] = pLSA_init(m,nd,K,Sindices,KbIndices)
% m  ... number of words (vocabulary size)
% nd ... number of documents
% K  ... number of topics
% Sindices ... indices of succeeding runs
% KbIndices ... indices of aspects chosen as bug representing
%
% Pz   ... P(z)
% Pd_z ... P(d|z)
% Pw_z ... P(w|z)

Pz = principled_randomize(K,1); 
C = sum(Pz);
Pz = Pz / C; 

% random assignment
Pd_z = principled_randomize(nd,K);   % word probabilities conditioned on topic
%zeros entries corresponding to succeeding runs and bug aspects
%if not doing constrained pLSA pass empty KbIndices
Pd_z(Sindices, KbIndices) = 0; 
Pd_z = normalizeColumns(Pd_z);

% random assignment
Pw_z = normalizeColumns(principled_randomize(m,K));


return;

% This function generates a random matrix such that all entries are non-zero
function P = principled_randomize(m,n)
    while 1
        P = rand(m,n);
        if nnz(P) == numel(P)
            break;
        else
            warning('Needed to regenerate matrix');
        end;
    end;
return;


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% (1) E step compute posterior on z,  
% Pw_z - # words by # aspects matrix
% Pd_z - # documents by # aspects matrix
% Pz - # aspects matrix
% I - from [I,J,V] = sparse(X)
% J - from [I,J,V] = sparse(X)
% K - number of aspects
% M - number of rows of X
% N - number of columns of X
% E - number of nonzero entries of X
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function Pz_dw = pLSA_Estep(Pw_z,Pd_z,Pz,I,J,K,M,N,E)

   C = spalloc(M,N,E);
   for k = 1:K;
      T = Pw_z(I,k).*Pd_z(J,k) * Pz(k);
      Pz_dw{k} = sparse(I,J,T,M,N,E); 
      C = C + Pz_dw{k};
   end;   

   % normalize posterior
   for k = 1:K;
      Pz_dw{k} = Pz_dw{k} .* invertNonZeros(C);
   end;   
return;


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  (2) M step, maximize log-likelihood
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function [Pw_z,Pd_z,Pz] = pLSA_Mstep(X,Pz_dw,K,Sindices,KbIndices)
   
   for k = 1:K;
      T = X .* Pz_dw{k};
      Pw_z(:,k) = sum(T,2);
      Pd_z(:,k) = sum(T,1)';  
   end;

   %zeros entries corresponding to succeeding runs and bug aspects
   %if not doing constrained pLSA pass empty KbIndices
   Pd_z(Sindices,KbIndices) = 0;
   Pw_z = full(Pw_z);
   Pd_z = full(Pd_z);
   
   Pz = sum(Pd_z,1);
   
   % normalize to sum to 1
   Pw_z = normalizeColumns(Pw_z);

   Pd_z = normalizeColumns(Pd_z);
   
   C = sum(Pz,2);
   if C == 0 
       error('Sum of probabilities of topics should be greater than 0.\n');
   end;
   Pz = Pz .* 1./C;

return;
