% function [Pw_z,Pd_z,Pz,Li] = pLSA_EM(X,Fixed_Pw_z,K,Learn)
%
% Probabilistic Latent semantic alnalysis (pLSA)
%
% Notation:
% X ... (m x nd) term-document matrix (observed data)
%       X(i,j) stores number of occurrences of word i in document j
%
% m  ... number of words (vocabulary size)
% nd ... number of documents
% K  ... number of topics
%
% Fixed_Pw_z ... fixed Pw_z density (for recognition only)
%                leave empty in learning
%                N.B. Must be of size m by K
%
% Learn ... Structure holding all settings 
%
% Li   ... likelihood for each iteration
% Pz   ... P(z)
% Pd_z ... P(d|z) 
% Pw_z ... P(w|z) corresponds to beta parameter in LDA
%
% Pz_wd ... P(z|w,d) posterior on z
%
% 
% References: 
% [1] Thomas Hofmann: Probabilistic Latent Semantic Analysis, 
% Proc. of the 15th Conf. on Uncertainty in Artificial Intelligence (UAI'99) 
% [2] Thomas Hofmann: Unsupervised Learning by Probabilistic Latent Semantic
% Analysis, Machine Learning Journal, 42(1), 2001, pp.177.196 
%
% Josef Sivic
% josef@robots.ox.ac.uk
% 30/7/2004
%
% Extended by Rob Fergus
% fergus@csail.mit.edu
% 03/10/05

function [Pw_z,Pd_z,Pz,Li] = pLSA_EM(X,Fixed_Pw_z,K,Learn)

%% small offset to avoid numerical problems  
ZERO_OFFSET = 1e-7;

%%% Default settings
if nargin<3
   Learn.Max_Iterations  = 100;
   Learn.Min_Likelihood_Change   = 1;   
   Learn.Verbosity = 0;
end;   

if Learn.Verbosity
   figure(1); clf;
   title('Log-likelihood');
   xlabel('Iteration'); ylabel('Log-likelihood');
end;

m  = size(X,1); % vocabulary size
nd = size(X,2); % # of documents

% initialize Pz, Pd_z,Pw_z
[Pz,Pd_z,Pw_z] = pLSA_init(m,nd,K);

% if in recognition, reset Pw_z to fixed distribution
% from learning phase....
if isempty(Fixed_Pw_z)
    %% learning mode
    FIXED_PW_Z = 0;
else
    %% recognition mode
    FIXED_PW_Z = 1;
    
    %%% check that the size is compatible
    if ((size(Pw_z,1)==size(Fixed_Pw_z,1)) & (size(Pw_z,2)==size(Fixed_Pw_z,2)))
        %% overwrite random Pw_z
        Pw_z = Fixed_Pw_z;
    else
        error('Dimensions of fixed Pw_z density do not match VQ.Codebook_Size and Learn.Num_Topics');
    end 
end


% allocate memory for the posterior
Pz_dw = zeros(m,nd,K); 

Li    = [];
maxit = Learn.Max_Iterations;

% EM algorithm
for it = 1:maxit   
   fprintf('Iteration %d ',it);
   
   % E-step
   Pz_dw = pLSA_Estep(Pw_z,Pd_z,Pz);
   
   % M-step
   [Pw_z,Pd_z,Pz] = pLSA_Mstep(X,Pz_dw);

   % if in recognition mode, reset Pw_z to fixed density
   if (FIXED_PW_Z)
       Pw_z = Fixed_Pw_z;
   end  
   
   if Learn.Verbosity>=2
     Pw_z
   end;  
    
   % Evaluate data log-likelihood
   Li(it) = pLSA_logL(X,Pw_z,Pz,Pd_z);   
        
   % plot loglikelihood
   if Learn.Verbosity>=3
      figure(ff(1));
      plot(Li,'b.-');
   end;
      
   %%% avoid numerical problems.
   Pw_z = Pw_z + ZERO_OFFSET;
   
   % convergence?
   dLi = 0;
   if it > 1
     dLi    = Li(it) - Li(it-1);
     if dLi < Learn.Min_Likelihood_Change, break; end;   
   end;
   fprintf('dLi=%f \n',dLi);
end;

fprintf('\n');

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% initialize conditional probabilities for EM 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function [Pz,Pd_z,Pw_z] = pLSA_init(m,nd,K)
% m  ... number of words (vocabulary size)
% nd ... number of documents
% K  ... number of topics
%
% Pz   ... P(z)
% Pd_z ... P(d|z)
% Pw_z ... P(w|z)

Pz   = ones(K,1)/K; % uniform prior on topics

% random assignment
Pd_z = rand(nd,K);   % word probabilities conditioned on topic
C    = 1./sum(Pd_z,1);  % normalize to sum to 1
Pd_z = Pd_z * diag(C);

% random assignment
Pw_z = rand(m,K);
C    = 1./sum(Pw_z,1);    % normalize to sum to 1
Pw_z = Pw_z * diag(C);


return;


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% (1) E step compute posterior on z,  
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function Pz_dw = pLSA_Estep(Pw_z,Pd_z,Pz)
   K = size(Pw_z,2);

   for k = 1:K
      Pz_dw(:,:,k) = Pw_z(:,k) * Pd_z(:,k)' * Pz(k);
   end;   
   C = sum(Pz_dw,3);

   % normalize posterior
   for k = 1:K
      Pz_dw(:,:,k) = Pz_dw(:,:,k) .* (1./C);
   end;   
return;


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  (2) M step, maximazize log-likelihood
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function [Pw_z,Pd_z,Pz] = pLSA_Mstep(X,Pz_dw)
   K = size(Pz_dw,3);
   
   for k = 1:K
      Pw_z(:,k) = sum(X .* Pz_dw(:,:,k),2);
   end;
   
   for k = 1:K
      Pd_z(:,k) = sum(X.* Pz_dw(:,:,k),1)';
   end;
   
   Pz = sum(Pd_z,1);
   
   % normalize to sum to 1
   C = sum(Pw_z,1);
   Pw_z = Pw_z * diag(1./C);

   C = sum(Pd_z,1);
   Pd_z = Pd_z * diag(1./C);
   
   C = sum(Pz,2);
   Pz = Pz .* 1./C;
   Pz;

return;


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% data log-likelihood
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function L = pLSA_logL(X,Pw_z,Pz,Pd_z)
   L = sum(sum(X .* log(Pw_z * diag(Pz) * Pd_z' + eps)));

return; 



return;

