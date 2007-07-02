%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% data log-likelihood
% LM ..sparse matrix containing non zero log likelihood elements
% L ...log likelihood value
% I ...row indices of X matrix
% J ...columns indices of X matrix
% V ...values of X matrix
% Pw_z 
% Pz
% Pd_z
% M ...number of rows of X
% N ...number of columns of X
% E ...number of nonzero elements of X
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function [L,LM] = pLSA_logL(I,J,V,Pw_z,Pz,Pd_z,M,N,E)
   LM = spalloc(M,N,E);   
   for i = 1:E
       LM(I(i),J(i)) = Pw_z(I(i), :) * diag(Pz) * Pd_z(J(i),:)'; 
   end;
   [IL,JL,L] = find(LM); 
   T = V .* log(L);
   if nnz(LM) ~= E 
       L = -Inf;   
       warning('Probability of word and document co-occurring was zero, but actual count was not.');
   else
       L = sum(T);
       LM = sparse(I,J,T);
   end;

return; 
