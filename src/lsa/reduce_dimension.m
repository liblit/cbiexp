load([curdir '/decomposition.mat']); 

% Select only the first 2 parts of each matrix
Uk = U(:,1:2);
Sk = S(1:2,1:2); 
Vk = V(:,1:2);

% Load failure and success vectors
Findices = load([curdir '/f.m']);
Sindices = load([curdir '/s.m']);

% Load data from oracle
% Pad resulting matrix with additional zeros so same size as base matrix 
Bsparse = load([curdir '/bug_runs.m']);
B = spconvert(Bsparse); clear Bsparse; B = full(B);
sB = size(B); sX = size(X); B = [B, zeros(sB(1,1), sX(1,2) - sB(1,2))];
clear sB; clear sX; 

% map each run into the two-dimensional space
R = inv(Sk) * Uk' * X;
T = inv(Sk) * Vk' * X';

% Build a vector to color scatter plot.
% The vector must be the same length as X has columns
% We expect that successful runs will be color 1 and additional colors will
% be allocated to each bug as identified by the oracle
C = size(B); C = C(1,1); 
C = linspace(1,64,C); C = diag(C); C = C * B;
C = sum(C);
C(C == 0) = 32 * ones(size(Sindices));

% graph as scatter plot  
scatter(R(1,:), R(2,:), 64, C, filled)  
